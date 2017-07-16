/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_FRACTIONAL_DELAY_BASE_HPP_INCLUDED
#define VISR_LIBRBBL_FRACTIONAL_DELAY_BASE_HPP_INCLUDED

#include <memory>

namespace visr
{
namespace rbbl
{

/**
 * Abstract base class for interpolation algorithms on discrete-time sequences.
 */
template <typename SampleType>
class FractionalDelayBase
{
public:
  virtual ~FractionalDelayBase();

  virtual void interpolate( SampleType const * basePointer,
                            SampleType * result
                            SampleType startDelay, SampleType endDelay,
                            SampleType startGain, SampleType endGain ) = 0;
};

/**
 * Factory method to 
 */
template <typename SampleType>
class FractionalDelayBase
{
public:
  static create( std::string const & name );

  template< class InterpolatorType >
  static void register( std::string const & name );

private:
  struct Creator
  {
    using CreateFunction = std::function< std::unique_ptr<FractionalDelayBase>(std::string const & name) >;

    explicit Creator( CreateFunction fcn );

    std::unique_ptr<FractionalDelayBase> create( ParameterConfigBase const & config ) const;
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

    static std::unique_ptr< construct()
    {
      return std::unique_ptr<FractionalDelayBase>( new InterpolatorType() );
    }
  };

  using CreatorTable = std::map<InterpolatorType, Creator >;

  static CreatorTable & creatorTable();
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_FRACTIONAL_DELAY_BASE_HPP_INCLUDED
