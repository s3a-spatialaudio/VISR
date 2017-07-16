/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "fractional_delay_factory.hpp"

#include "fractional_delay_base.hpp"

#include "lagrange_interpolator.hpp"

namespace visr
{
namespace rbbl
{

namespace // unnamed
{

template< typename SampleType >
class TableInitializer
{
public:
  TableInitializer()
  {
    FractionalDelayFactory<SampleType>::template registerAlgorithm<LagrangeInterpolator<SampleType, 0> >( "nearestSample");
    FractionalDelayFactory<SampleType>::template registerAlgorithm<LagrangeInterpolator<SampleType, 0> >( "lagrangeOrder0");
    FractionalDelayFactory<SampleType>::template registerAlgorithm<LagrangeInterpolator<SampleType, 1> >( "lagrangeOrder1");
    FractionalDelayFactory<SampleType>::template registerAlgorithm<LagrangeInterpolator<SampleType, 2> >( "lagrangeOrder2" );
    FractionalDelayFactory<SampleType>::template registerAlgorithm<LagrangeInterpolator<SampleType, 3> >( "lagrangeOrder3" );
    FractionalDelayFactory<SampleType>::template registerAlgorithm<LagrangeInterpolator<SampleType, 4> >( "lagrangeOrder4");
    FractionalDelayFactory<SampleType>::template registerAlgorithm<LagrangeInterpolator<SampleType, 5> >( "lagrangeOrder5");
    FractionalDelayFactory<SampleType>::template registerAlgorithm<LagrangeInterpolator<SampleType, 6> >( "lagrangeOrder6");
    FractionalDelayFactory<SampleType>::template registerAlgorithm<LagrangeInterpolator<SampleType, 7> >( "lagrangeOrder7" );
    FractionalDelayFactory<SampleType>::template registerAlgorithm<LagrangeInterpolator<SampleType, 8> >( "lagrangeOrder8" );
    FractionalDelayFactory<SampleType>::template registerAlgorithm<LagrangeInterpolator<SampleType, 9> >( "lagrangeOrder9" );
  }
};

static TableInitializer<float> initializer;

} // unnamed namespace

template< class SampleType >
typename FractionalDelayFactory<SampleType>::CreatorTable & FractionalDelayFactory<SampleType>::creatorTable()
{
  static CreatorTable sTable;
  return sTable;
}

/**
 * Factory implementation
 */
template <typename SampleType>
std::unique_ptr<FractionalDelayBase<SampleType> > FractionalDelayFactory<SampleType>::create( std::string const & name,
                                                                                              std::size_t maxNumSamples )
{
  typename CreatorTable::const_iterator findIt = creatorTable().find( name );
  return findIt == creatorTable().end()
    ? std::unique_ptr<FractionalDelayBase<SampleType> >( nullptr )
    : findIt->second.create( maxNumSamples );
}

template <typename SampleType>
FractionalDelayFactory<SampleType>::Creator::Creator( CreateFunction fcn )
  : mCreateFunction( fcn )
{
}

template <typename SampleType>
std::unique_ptr<FractionalDelayBase<SampleType> > FractionalDelayFactory<SampleType>::Creator::create( std::size_t maxNumSamples ) const
{
  return mCreateFunction( maxNumSamples );
}

// Explicit instantiations
template class FractionalDelayFactory<float>;
template class FractionalDelayFactory<double>;

} // namespace rbbl
} // namespace visr
