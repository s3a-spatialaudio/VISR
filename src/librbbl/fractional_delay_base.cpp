/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "fractional_delay_base.hpp"

#include "lagrange_interpolator.hpp"

namespace visr
{
namespace rbbl
{

template <typename SampleType>
FractionalDelayBase<SampleType>::~FractionalDelayBase() = default;

namespace // unnamed
{

template< typename SampleType >
class TableInitializer: public FractionalDelayFactory<SampleType>::CreatorTable
{
public:
  TableInitializer()
  {
    registerAlgorithm<LagrangeInterpolator<SampleType,1> >("lagrangeOrder1");
    registerAlgorithm<LagrangeInterpolator<SampleType, 2> >( "lagrangeOrder2" );
    registerAlgorithm<LagrangeInterpolator<SampleType, 3> >( "lagrangeOrder3" );
  }
};

} // unnamed namespace

template< class SampleType >
typename FractionalDelayFactory<SampleType>::CreatorTable & FractionalDelayFactory<SampleType>::creatorTable()
{
  static typename CreatorTable sTable{}; // TableInitializer() };
  return sTable;
}

/**
 * Factory implementation
 */
template <typename SampleType>
std::unique_ptr<FractionalDelayBase<SampleType> > FractionalDelayFactory<SampleType>::create( std::string const & name,
                                                                                              std::size_t maxNumSamples )
{
  CreatorTable::const_iterator findIt = creatorTable().find( name );
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
template class FractionalDelayBase<float>;
template class FractionalDelayBase<double>;

template class FractionalDelayFactory<float>;
template class FractionalDelayFactory<double>;

} // namespace rbbl
} // namespace visr
