/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_TIME_FREQUENCY_TRANSFORM_HPP_INCLUDED
#define VISR_LIBRCL_TIME_FREQUENCY_TRANSFORM_HPP_INCLUDED

#include <libril/atomic_component.hpp>
#include <libril/constants.hpp>
#include <libril/audio_input.hpp>
#include <libril/parameter_output_port.hpp>

#include <libefl/aligned_array.hpp>

#include <libpml/time_frequency_parameter.hpp>
#include <libpml/shared_data_protocol.hpp>

//#include <librbbl/circular_buffer.hpp>

#include <cstddef> // for std::size_t
#include <memory>
#include <valarray>

namespace visr
{

// Forward declarations
namespace rbbl
{
template< typename SampleType >
class FftWrapperBase;

template< typename SampleType >
class CircularBuffer;

}

namespace rcl
{

class TimeFrequencyTransform: public AtomicComponent
{
  using SampleType = visr::SampleType;
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit TimeFrequencyTransform( SignalFlowContext& context,
                                   char const * name,
                                   CompositeComponent * parent = nullptr );
  
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

  AudioInput mInput;

  /**
   * Buffer for teporary storage of the input channel pointers.
   * @todo Change if the delay line supports an (optional) stride-based interface.
   */
  std::valarray<SampleType const *> mInputChannels;

  /**
   * Port is configured in the setup() 
   */
  std::unique_ptr<ParameterOutputPort<pml::SharedDataProtocol, pml::TimeFrequencyParameter<SampleType> > > mOutput;

  /**
   * 
   */
  std::size_t mNumberOfChannels;

  /**
   * The length of the Fourier transform;
   */
  std::size_t mDftlength;

  std::size_t mWindowLength;

  std::size_t mDftSamplesPerPeriod;

  std::size_t mHopSize;

  std::unique_ptr< rbbl::CircularBuffer<SampleType> > mInputBuffer;

  /**
   * Wrapper for the different FFT libraries
   */
  std::unique_ptr< rbbl::FftWrapperBase<SampleType> > mFftWrapper;

  std::size_t mAlignment;

  /**
   * The window 
   */
  efl::AlignedArray<SampleType> mWindow;

  efl::AlignedArray<SampleType> mCalcBuffer;
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_TIME_FREQUENCY_TRANSFORM_HPP_INCLUDED
