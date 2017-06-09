/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_FRACTIONAL_DELAY_FACTORY_HPP_INCLUDED
#define VISR_LIBRBBL_FRACTIONAL_DELAY_FACTORY_HPP_INCLUDED

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace visr
{
namespace rbbl
{

// Forward declarations
template< typename SampleType >
class FractionalDelayBase;

/**
 * Factory class to instantiate fractional delay interpolation algorithms.
 *
 */
template <typename SampleType>
class FractionalDelayFactory
{
public:
  static std::unique_ptr<FractionalDelayBase<SampleType> > create( std::string const & name,
                                                                   std::size_t maxNumSamples );

  template< class InterpolatorType >
  static void registerAlgorithm( std::string const & name );

private:
  struct Creator
  {
    using CreateFunction = std::function< std::unique_ptr<FractionalDelayBase<SampleType> >(std::size_t) >;

    explicit Creator( CreateFunction fcn );

    std::unique_ptr<FractionalDelayBase<SampleType> > create( std::size_t maxNumSamples ) const;
  private:
    CreateFunction mCreateFunction;
  };

  template< class InterpolatorType >
  class TCreator: public Creator
  {
  public:
    TCreator()
      : Creator( &TCreator<InterpolatorType>::construct )
    {
    }

    static std::unique_ptr<FractionalDelayBase<SampleType> > construct( std::size_t maxNumSamples )
    {
      return std::unique_ptr<FractionalDelayBase<SampleType> >( new InterpolatorType( maxNumSamples ) );
    }
  };

  using CreatorTable = std::map<std::string const, Creator >;

  static CreatorTable & creatorTable();
};

template< class SampleType >
template< class InterpolatorType >
void FractionalDelayFactory<SampleType>::registerAlgorithm( std::string const & name )
{
  creatorTable().insert( std::make_pair( name, TCreator<InterpolatorType>() ) );
}

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_FRACTIONAL_DELAY_FACTORY_HPP_INCLUDED
