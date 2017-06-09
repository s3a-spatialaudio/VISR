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
#include <valarray>

namespace visr
{
namespace rbbl
{
template< typename SampleType >
class MultichannelConvolverUniform;
}

namespace rcl
{

/**
 * Audio Component for creating a set of decorrelated signals out of a single input signal.
 */
class SingleToMultichannelDiffusion: public AtomicComponent
{
  /**
   * Alias for the type of the audio samples processed by this component.
   */
  using SampleType = visr::SampleType;
public:
  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component.
   */
  explicit SingleToMultichannelDiffusion( SignalFlowContext const & context,
                                          char const * name,
                                          CompositeComponent * parent = nullptr );
    
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
  AudioInput mInput;

  /**
   * The audio output, width \p numberOfOutputs.
   */
  AudioOutput mOutput;

  /**
   * The number of output channels.
   */
  std::size_t mNumberOfOutputs;

  /**
   * A one-to-N FIR filter for diffusion.
   */
  std::unique_ptr<rbbl::MultichannelConvolverUniform<SampleType> > mDiffusionFilter;

  /**
   * Buffer to hold the pointers to the channels of the output port.
   * @todo Change if the MultichannelConvolver class offers a stride-based interface
   */
  std::valarray<SampleType*> mOutputChannels;
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_SINGLE_TO_MULTI_CHANNEL_DIFFUSION_HPP_INCLUDED
