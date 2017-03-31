/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_TIME_FREQUENCY_INVERSE_TRANSFORM_HPP_INCLUDED
#define VISR_LIBRCL_TIME_FREQUENCY_INVERSE_TRANSFORM_HPP_INCLUDED

#include <libril/atomic_component.hpp>
#include <libril/constants.hpp>
#include <libril/audio_output.hpp>
#include <libril/parameter_input.hpp>

#include <libefl/aligned_array.hpp>
#include <libefl/basic_matrix.hpp>

#include <libpml/time_frequency_parameter.hpp>
#include <libpml/shared_data_protocol.hpp>

#include <cstddef> // for std::size_t
#include <memory>

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

class TimeFrequencyInverseTransform: public AtomicComponent
{
  using SampleType = visr::SampleType;
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit TimeFrequencyInverseTransform( SignalFlowContext& context,
                                   char const * name,
                                   CompositeComponent * parent = nullptr );
  ~TimeFrequencyInverseTransform();

  /**
   * @todo Add parameter to describe the window type.
   */
  void setup( std::size_t numberOfChannels, 
              std::size_t dftLength,
              std::size_t hopSize,
              char const * fftImplementation = "default" );

  void process( );

private:
  std::size_t mAlignment;

  AudioOutput mOutput;

  /**
   * Port is configured in the setup() 
   */
  std::unique_ptr<ParameterInput<pml::SharedDataProtocol, pml::TimeFrequencyParameter<SampleType> > > mInput;

  /**
   * 
   */
  std::size_t mNumberOfChannels;

  /**
   * The length of the Fourier transform;
   */
  std::size_t mDftLength;

  std::size_t mDftSamplesPerPeriod;

  std::size_t mHopSize;

  // std::unique_ptr< rbbl::CircularBuffer<SampleType> > mAccumulationBuffer;

  efl::BasicMatrix<SampleType> mAccumulationBuffer;

  /**
   * Wrapper for the different FFT libraries
   */
  std::unique_ptr< rbbl::FftWrapperBase<SampleType> > mFftWrapper;

  efl::AlignedArray<SampleType> mCalcBuffer;
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_TIME_FREQUENCY_INVERSE_TRANSFORM_HPP_INCLUDED
