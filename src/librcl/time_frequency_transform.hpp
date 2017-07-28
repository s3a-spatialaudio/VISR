/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_TIME_FREQUENCY_TRANSFORM_HPP_INCLUDED
#define VISR_LIBRCL_TIME_FREQUENCY_TRANSFORM_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libril/atomic_component.hpp>
#include <libril/constants.hpp>
#include <libril/audio_input.hpp>
#include <libril/parameter_output.hpp>

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

  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component
   * @param numberOfChannels The number of distinct audio waveforms received through the input audio port.
   */
  explicit TimeFrequencyTransform( SignalFlowContext const & context,
                                   char const * name,
                                   CompositeComponent * parent,
                                   std::size_t numberOfChannels,
                                   std::size_t dftLength,
                                   std::size_t windowLength,
                                   std::size_t hopSize,
                                   char const * fftImplementation = "default" );

  explicit TimeFrequencyTransform( SignalFlowContext const & context,
                                   char const * name,
                                   CompositeComponent * parent,
                                   std::size_t numberOfChannels,
                                   std::size_t dftLength,
                                   efl::BasicVector<SampleType> const & window,
                                   std::size_t hopSize,
                                   char const * fftImplementation = "default" );


  ~TimeFrequencyTransform();

  /**
   * @todo Add parameter to describe the window type.
   */
  void setup( std::size_t numberOfChannels, 
              std::size_t dftLength,
              std::size_t windowLength,
              std::size_t hopSize,
              char const * fftImplementation = "default" );

  void process( );

private:

  std::size_t const mAlignment;

  /**
   * 
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
