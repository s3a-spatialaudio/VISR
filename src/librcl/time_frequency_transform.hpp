/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_TIME_FREQUENCY_TRANSFORM_HPP_INCLUDED
#define VISR_LIBRCL_TIME_FREQUENCY_TRANSFORM_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/atomic_component.hpp>
#include <libvisr/constants.hpp>
#include <libvisr/audio_input.hpp>
#include <libvisr/parameter_output.hpp>

#include <libefl/basic_vector.hpp>

#include <libpml/time_frequency_parameter.hpp>
#include <libpml/shared_data_protocol.hpp>

#include <librbbl/circular_buffer.hpp>

#include <cstddef> // for std::size_t
#include <memory>

namespace visr
{

// Forward declarations
namespace rbbl
{
template< typename SampleType >
class FftWrapperBase;
}

namespace rcl
{

/**
 * Component to transform a multichannel input audio signal into a sequence of time-frequency parameter data.
 */
class VISR_RCL_LIBRARY_SYMBOL TimeFrequencyTransform: public AtomicComponent
{
public:
  using SampleType = visr::SampleType;

  enum class Normalisation
  {
    One,      /**< No normalisation applied. */
    Unitary,  /**< Normalisation (scaling) factor 1/sqrt(N) */
    OneOverN  /**< Scaling factor 1/N, which means that the corresponding inverse FFT has a normalisation of "One". */
  };

  /**
   * Constructor. Creates a time-frequency transformation component with a default window shape.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component
   * @param numberOfChannels The number of distinct audio waveforms received through the input audio port.
   * @param dftLength The size of the DFT transform used. Must be a power of two for most FFT implementations.
   * @param windowLength The length of the window. A Hann window modified to satisfy the Constant Overlap-Add (COLA) property is used.
   * @param hopSize Advance (in samples) between successive frames. The component's period size must be an ineger multiple of the hop size.
   * @param fftImplementation String desribing the FFT implementation to be used. Optional parameter, defaults to the
   * platform's default FFT implementation.
   * @param normalisation The DFT normalisation policy, defaults to 'Unitary'
   */
  explicit TimeFrequencyTransform( SignalFlowContext const & context,
                                   char const * name,
                                   CompositeComponent * parent,
                                   std::size_t numberOfChannels,
                                   std::size_t dftLength,
                                   std::size_t windowLength,
                                   std::size_t hopSize,
                                   char const * fftImplementation = "default",
                                   Normalisation normalisation = Normalisation::Unitary );

  /**
  * Constructor. Creates a time-frequency transformation component with a user-defined window shape.
  * @param context Configuration object containing basic execution parameters.
  * @param name The name of the component. Must be unique within the containing composite component (if there is one).
  * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component
  * @param numberOfChannels The number of distinct audio waveforms received through the input audio port.
  * @param dftLength The size of the DFT transform used. Must be a power of two for most FFT implementations.
  * @param window The coefficients of the window applied to the time-domain input frames.
  * @param hopSize Advance (in samples) between successive frames. The component's period size must be an ineger multiple of the hop size.
  * @param fftImplementation String desribing the FFT implementation to be used. Optional parameter, defaults to the
  * platform's default FFT implementation.
  */
  explicit TimeFrequencyTransform( SignalFlowContext const & context,
                                   char const * name,
                                   CompositeComponent * parent,
                                   std::size_t numberOfChannels,
                                   std::size_t dftLength,
                                   efl::BasicVector<SampleType> const & window,
                                   std::size_t hopSize,
                                   char const * fftImplementation = "default",
                                   Normalisation normalisation = Normalisation::Unitary );

  /**
   * Destructor (virtual)
   */
  ~TimeFrequencyTransform() override;

  void process( ) override;

private:

  std::size_t const mAlignment;

  /**
   * The number of simulataneously processed audio channels.
   */
  std::size_t const mNumberOfChannels;

  /**
   * The length of the Fourier transform;
   */
  std::size_t const mDftlength;

  std::size_t const mWindowLength;

  std::size_t mDftSamplesPerPeriod;

  std::size_t const mHopSize;

  rbbl::CircularBuffer<SampleType> mInputBuffer;

  /**
   * Wrapper for the different FFT libraries
   */
  std::unique_ptr< rbbl::FftWrapperBase<SampleType> > mFftWrapper;

  /**
   * The window 
   */
  efl::BasicVector<SampleType> mWindow;

  efl::BasicVector<SampleType> mCalcBuffer;

  AudioInput mInput;

  /**
   * 
   */
  ParameterOutput<pml::SharedDataProtocol, pml::TimeFrequencyParameter<SampleType> > mOutput;
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_TIME_FREQUENCY_TRANSFORM_HPP_INCLUDED
