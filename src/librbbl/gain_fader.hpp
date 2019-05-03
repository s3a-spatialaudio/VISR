/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_GAIN_FADER_HPP_INCLUDED
#define VISR_LIBRBBL_GAIN_FADER_HPP_INCLUDED

#include "export_symbols.hpp"

#include<libefl/aligned_array.hpp>
#include<libefl/basic_matrix.hpp>

namespace visr
{
namespace rbbl
{

/**
 * Low-level Utility class to apply a linearly interpolated gain change to an audio signal.
 * The class itself is stateless, i.e., it can be used for an arbitrary number of audio signals.
 * @tparam ElementType The data type used for the audio samples and the gain values. 
 * The class template is explicitly instantiated for element types float and double.
 */
template< typename ElementType >
class VISR_RBBL_LIBRARY_SYMBOL GainFader
{
public:
  /**
   * Constructor, sets up the internal data structures
   * @param blockSize The number of samples processed in a single process() call.
   * @param interpolationSteps The number of samples used to interpolate to the end value.
   * @param alignment the minimum alignment for the input and output vectors as well as the internally stored data members,
   * given as a number of samples.
   */
  explicit GainFader( std::size_t blockSize,
                      std::size_t interpolationSteps,
                      std::size_t alignment = 0 );

  /**
   * Destructor.
   */
  ~GainFader();

  /**
  * Return the number of interpolation steps, i.e., the number of samples it takes to reach a newly set gain value..
  */
  std::size_t interpolationSamples() const;

  /**
   * Return the number of interpolation 'buffers'/blocks, i.e., the next integer multiple of the 
   * blockSize equal or larger than the number of interpolation samples.
   */
  std::size_t interpolationPeriods() const;

  /**
   * Process \p mBlockSize samples of a single audio signal. The gain trajectory depends on the start end end gain
   * values and the time into the current transition (denoted by \p blockIndex). This method does not alter the state of the fader (and is therefore const).
   * @param input a buffer containing input samples. Must provide \p mBlockSize elements.
   * sample vector must contain at least \p blockLength elements.
   * @param[out] output Array to write the scaled output signals. Must provide space for \p mBlockSize samples.
   * @param startGain Gain value at the begin of the transition (the complete transition, not the current block)
   * @param endGain Gain value at the end of the transition (the complete transition, not the current block)
   * @param blockIndex block number denoting the current position within the current transition. In this invocation of process(), the section
   * [blockIndex*mBlockSize..(blockIndex+1)*mBlockSize] of the interpolation ramp is applied. A value equal or larger than \p mInterpolationPeriods
   * means that the signal is scaled by the constant value \p endGain.
   */
  void scale( ElementType const * input, ElementType * output,
              ElementType startGain,
              ElementType endGain,
              std::size_t blockIndex ) const;

  /**
  * Process \p mBlockSize samples of a single audio signal and add to the output signal. The gain trajectory depends on the start end end gain
  * values and the time into the current transition (denoted by \p blockIndex). This method does not alter the state of the fader (and is therefore const).
  * @param input a buffer containing input samples. Must provide \p mBlockSize elements.
  * sample vector must contain at least \p blockLength elements.
  * @param[in,out] outputAcc Array to write the scaled output signals. Must provide space for \p mBlockSize samples.
  * @param startGain Gain value at the begin of the transition (the complete transition, not the current block)
  * @param endGain Gain value at the end of the transition (the complete transition, not the current block)
  * @param blockIndex block number denoting the current position within the current transition. In this invocation of process(), the section
  * [blockIndex*mBlockSize..(blockIndex+1)*mBlockSize] of the interpolation ramp is applied. A value equal or larger than \p mInterpolationPeriods
  * means that the signal is scaled by the constant value \p endGain.
  */
  void scaleAndAccumulate( ElementType const * input, ElementType * outputAcc,
    ElementType startGain,
    ElementType endGain,
    std::size_t blockIndex ) const;


private:

  /**
   * Private implementation method to set up the interpolation ramp.
   * This array is initialised as a linear sequence from 0.0 (not included) to 1.0 over the course of interpolationStep samples.
   * the remainder of the ramp is filled with value 1.0 to enforce a constant scaling with the gain end value after the
   * transition is complete.
   * @param blockSize Number of samples processed in one process() call.
   * @param interpolationSteps Number of samples it takes to transistion 
   * to the new gain value. Not required to be a multiple of \p blockSize.
   */
  void setupRamp( std::size_t blockSize,
                  std::size_t interpolationSteps );

  /**
   * The number of samples processed in each invocation of process.
   */
  std::size_t const mBlockSize;

  /**
  * The number of samples it takes to transition to a new gain values.
  * 0 denotes an immediate application of a new gain matrix.
  */
  std::size_t const mInterpolationSamples;

  /**
   * The number of process() invocations needed to perform a complete transition of gain values(),
   * i.e., the first number of blocks that contain interpolationSteps samples.
   */
  std::size_t const mInterpolationPeriods;

  /**
   * Precomputed vector of scaling values for computing the output values using interpolated gains.
   * It consists of a linear ramp from 0 to 1 over \p interpolationSteps samples, followed by
   * at least \p mBlockSize values of '1' for the static gain after the transition has finished.
   * Length: \p (mInterpolationSteps+1)*mBlockSize
   */
  efl::AlignedArray< ElementType > mInterpolationRamp;

  /**
   * Sample array used for temporary results within a process() call.
   * Length: blockSize
   */
  mutable efl::AlignedArray< ElementType > mTempBuffer;

};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_GAIN_FADER_HPP_INCLUDED
