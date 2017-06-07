/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_MULTICHANNEL_DELAY_LINE_HPP_INCLUDED
#define VISR_LIBRBBL_MULTICHANNEL_DELAY_LINE_HPP_INCLUDED

#include "circular_buffer.hpp"
#include "fractional_delay_base.hpp"

#include <libril/constants.hpp>

#include <cassert>
#include <complex>
#include <memory>
#include <vector>

namespace visr
{
namespace rbbl
{

/**
 * Generic class for MIMO convolution using a uniformly partioned fast convolution algorithm.
 * It supports a arbitrary numbers of input and output channels and enables filters and an
 * optional gain factor for arbitrary input-output connections.
 * @tparam SampleType The floating-point type of the signal samples
 */
template< typename SampleType >
class MultichannelDelayLine
{
public:
  /**
   * Enumeration for how to incorporate the implementation delay of the method into the interpolation.
   * The implementation (or method) delay is the delay introduced by the fractional delay algorithm itself.
   */
  enum class MethodDelayPolicy: char
  {
    Add, /**< Always add the implementation delay to the nominal amount of delay. */
    Limit, /**< Compensate the implementation delay, but limit to the minimum admissible delay if the nominal delay
    is smaller than the implementation delay */
    Reject /**< Throw an exception if the nominal delay value is lower than the method delay. */
  };

  /**
  * Constructor.
  * @param numberOfInputs The number of input signals processed.
  * @param numberOfOutputs The number of output channels produced.
  * @param blockLength The numbers of samples processed for each input or output channels in one process() call.
  * @param maxFilterLength The maximum length of the FIR filters (in samples).
  * @param maxRoutingPoints The maximum number of routing points between input and output channels, i.e., the number of filter operations to be calculated.
  * @param maxFilterEntries The maximum number of filters that can be stored within the filter. This number can be different from \p maxRoutingPoints, as the convolver can 
  * both reuse the same filter representation multiple times, or store multiple filter representations to enable switching of the filter characteristics at runtime.
  * @param initialRoutings The initial set of routing points.
  * @param initialFilters The initial set of filter coefficients. The matrix rows represent the distinct filters.
  * @param alignment The alignment (given as a multiple of the sample type size) to be used to allocate all data structure. It also guaranteees 
  * the alignment of the input and output samples to the process call. 
  * @param fftImplementation A string to determine the FFT wrapper to be used. The default value results in using the default FFT implementation for the given data type.
  */
  explicit MultichannelDelayLine( std::size_t numberChannels,
                                  SamplingFrequencyType samplingFrequency,
                                  std::size_t blockLength,
                                  SampleType maxDelaySeconds,
                                  char const * interpolationMethod,
                                  MethodDelayPolicy methodDelayPolicy = MethodDelayPolicy::Add,
                                  std::size_t alignment = 0 );

  ~MultichannelDelayLine();

  std::size_t numberOfChannels() const;

  std::size_t blockLength() const;

  void write( SampleType const * input,
              std::size_t channelStride,
              std::size_t numberOfChannels,
              std::size_t alignment );

  void interpolate( SampleType * output,
                    std::size_t channelIdx,
                    std::size_t numberOfSamples,
                    SampleType startDelay, SampleType endDelay,
                    SampleType startGain, SampleType endGain );

private:
  /**
   * Adjust the delay for the method delay of the interpolator depending on the chosen policy.
   * @throw std::out_of_range if the
   * @throw std::out_of_range if the delay value is lower than the method delay and the "reject" policy is chosen.
   */
  SampleType adjustDelay( SampleType rawDelay ) const;

  std::size_t const cNumberOfChannels;

  std::size_t const cBlockLength;

  SampleType const cSamplingFrequency;

  SampleType const cMaxDelaySamples;

  MethodDelayPolicy const cMethodDelayPolicy;

  rbbl::CircularBuffer<SampleType> mRingbuffer;

  std::unique_ptr<FractionalDelayBase<SampleType> > mInterpolator;

  SampleType cMethodDelay;
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_MULTICHANNEL_DELAY_LINE_HPP_INCLUDED
