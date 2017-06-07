/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_FRACTIONAL_DELAY_BASE_HPP_INCLUDED
#define VISR_LIBRBBL_FRACTIONAL_DELAY_BASE_HPP_INCLUDED

#include <cstddef>

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

  /**
   * Return the inherent implementation delay of the interpolator (in samples)
   */
  virtual SampleType methodDelay() const = 0;

  virtual void interpolate( SampleType const * basePointer,
                            SampleType * result,
                            std::size_t numSamples,
                            SampleType startDelay, SampleType endDelay,
                            SampleType startGain, SampleType endGain ) = 0;
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_FRACTIONAL_DELAY_BASE_HPP_INCLUDED
