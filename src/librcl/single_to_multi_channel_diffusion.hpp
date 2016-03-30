/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_SINGLE_TO_MULTI_CHANNEL_DIFFUSION_HPP_INCLUDED
#define VISR_LIBRCL_SINGLE_TO_MULTI_CHANNEL_DIFFUSION_HPP_INCLUDED

#define DIFFUSION_USE_FAST_CONVOLVER

#include <libril/atomic_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>

#include <libefl/basic_vector.hpp>
#include <libefl/basic_matrix.hpp>

#include <cstddef> // for std::size_t
#include <memory>

namespace visr
{
namespace rbbl
{
#ifdef DIFFUSION_USE_FAST_CONVOLVER
template< typename SampleType >
class MultichannelConvolverUniform;
#else
class FIR;
#endif
}

namespace rcl
{

/**
 * Audio Component for creating a set of decorrelated signals out of a single input signal.
 */
class SingleToMultichannelDiffusion: public ril::AtomicComponent
{
  /**
   * Alias for the type of the audio samples processed by this component.
   */
  using SampleType = ril::SampleType;
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit SingleToMultichannelDiffusion( ril::AudioSignalFlow& container, char const * name );
    
  ~SingleToMultichannelDiffusion();

  /**
   * Setup method to initialise the object and set the parameters.
   * @param numberOfOutputs The number of signals in the output signal.
   * @param diffusionFilters A matrix of FIR coefficients, where each row corresponds to a single filter.
   * @param gainAdjustments A vector of gains (linear scale) for the individual output channels. Must contain \p numberOfOutputs elements.
   * @todo Add and document any additional parameters needed by the diffusion algorithm.
   */
  void setup( std::size_t numberOfOutputs,
              efl::BasicMatrix<SampleType> const & diffusionFilters,
              efl::BasicVector<SampleType> const & gainAdjustments ); // NOTE: This parameter must go at the end.

  /**
  * Setup method to initialise the object and set the parameters.
  * @param numberOfOutputs The number of signals in the output signal.
  * @param diffusionFilters A matrix of FIR coefficients, where each row corresponds to a single filter.
  * @param globalGainAdjustment A gain adjustment (linear scale) set for all output channels.Defaults to 1.0.
  * @todo Add and document any additional parameters needed by the diffusion algorithm.
  */
  void setup( std::size_t numberOfOutputs,
              efl::BasicMatrix<SampleType> const & diffusionFilters,
              SampleType globalGainAdjustment = static_cast<SampleType>(1.0) ); // NOTE: This parameter must go at the end.


  /**
   * The process function, takes a block of data from the single input signal and calculates a block of 
   * decorrelated output data for each output channel.
   */
  void process( );

private:

  /**
   * The audio input, width 1.
   */
  ril::AudioInput mInput;

  /**
   * The audio output, width \p numberOfOutputs.
   */
  ril::AudioOutput mOutput;

  /**
   * The number of output channels.
   */
  std::size_t mNumberOfOutputs;

#ifndef DIFFUSION_USE_FAST_CONVOLVER
  /**
   * Gain adjustment levels (linear scale) for each output channel. 
   */
  efl::BasicVector<SampleType> mGainAdjustments;
#endif

  /**
   * An one-to-N FIR filter for diffusion.
   */
#ifdef DIFFUSION_USE_FAST_CONVOLVER
  std::unique_ptr<rbbl::MultichannelConvolverUniform<ril::SampleType> > mDiffusionFilter;
#else
  std::unique_ptr<rbbl::FIR> mDiffusionFilter;
  /**
   * Output matrix for the results of the filtering operation.
   * @note Only needed with the rbbl::FIR class.
   */
  efl::BasicMatrix<SampleType> mFilterOutputs;
#endif

  /**
   * Buffer to hold the pointers into the output buffers for the filtering.
   */
  std::vector<SampleType*> mOutputPointers;
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_SINGLE_TO_MULTI_CHANNEL_DIFFUSION_HPP_INCLUDED
