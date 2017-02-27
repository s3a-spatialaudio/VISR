/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librrl/audio_signal_flow.hpp> 

#include <libril/component.hpp>

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#else
#include <boost/python.hpp>
#include <boost/python/args.hpp>
#include <boost/noncopyable.hpp>
#endif

#include <vector>

namespace visr
{

using rrl::AudioSignalFlow;

namespace python
{
namespace rrl
{

#ifdef USE_PYBIND11


pybind11::array_t<SampleType> wrapProcess( visr::rrl::AudioSignalFlow & flow, pybind11::array const & input )
{
  using DataType = SampleType; // Possibly replace by a template parameter later.

  if( input.dtype() != pybind11::dtype::of<DataType>() )
  {
    throw std::invalid_argument( "AudioSignalFlow::process(): The data type input matrix does not match the used sample data type." );
  }
  if( input.ndim() != 2 )
  {
    throw std::invalid_argument( "AudioSignalFlow::process(): The input matrix is not 2D" );
  }
  if( input.shape(0) != flow.numberOfCaptureChannels() )
  if( input.shape(1) != flow.period() )
  {
    throw std::invalid_argument( "AudioSignalFlow::process(): Dimension 1 input of the input matrix does not match the block size of the signal flow." );
  }
  if( input.strides(1) != sizeof(SampleType) )
  {
    throw std::invalid_argument( "AudioSignalFlow::process(): Dimension 1 input of the input matrix must be continuous (row-major)." );
  }
  std::vector<SampleType const *> inPtrs( flow.numberOfCaptureChannels(), nullptr );
  std::size_t const inChannelStride = input.strides(0) / sizeof(DataType);
  SampleType const * inPtr = static_cast<SampleType const *>( input.data() );
  for( auto & el : inPtrs )
  {
    el = inPtr;
    inPtr += inChannelStride;
  }

  // Allocate the output array.
  pybind11::array outputSignal( pybind11::dtype::of<SampleType>(),
				{ flow.numberOfPlaybackChannels(), flow.period() },
				{ sizeof(SampleType)*flow.period(), sizeof(SampleType) } // TODO: Take care of alignment
                              );
  std::vector<SampleType *> outPtrs( flow.numberOfPlaybackChannels(), nullptr );
  std::size_t const outChannelStride = outputSignal.strides(0) / sizeof(DataType);
  SampleType * outPtr = static_cast<SampleType *>( outputSignal.mutable_data() );
  for( auto & el : outPtrs )
  {
    el = outPtr;
    outPtr += outChannelStride;
  }

  flow.process( &inPtrs[0], &outPtrs[0] );

  return outputSignal;
}

void exportAudioSignalFlow( pybind11::module & m )
{
  pybind11::class_<AudioSignalFlow>( m, "AudioSignalFlow" )
   .def( pybind11::init<visr::Component&>() )
   .def_property_readonly( "initialised", &AudioSignalFlow::initialised )
   .def_property_readonly( "numberOfAudioCapturePorts", &AudioSignalFlow::numberOfAudioCapturePorts )
   .def_property_readonly( "numberOfAudioPlaybackPorts", &AudioSignalFlow::numberOfAudioPlaybackPorts )
   .def_property_readonly( "numberOfCaptureChannels", &AudioSignalFlow::numberOfCaptureChannels )
   .def_property_readonly( "numberOfPlaybackChannels", &AudioSignalFlow::numberOfPlaybackChannels )
   .def( "audioCapturePortName", &AudioSignalFlow::audioCapturePortName, pybind11::arg("index"), pybind11::return_value_policy::reference )
   .def( "audioPlaybackPortName", &AudioSignalFlow::audioPlaybackPortName, pybind11::arg( "index" ), pybind11::return_value_policy::reference )
   .def( "process", [](visr::rrl::AudioSignalFlow & flow, pybind11::array const & input) /*-> pybind11::array_t<SampleType>*/ { return wrapProcess( flow, input );}, pybind11::return_value_policy::take_ownership )
  ;
}


#else

using namespace boost::python;


void exportAudioSignalFlow()
{
  class_<AudioSignalFlow, boost::noncopyable>( "AudioSignalFlow", init<visr::Component&>() )
    .add_property( "initialised", &AudioSignalFlow::initialised )
    .add_property( "numberOfAudioCapturePorts", &AudioSignalFlow::numberOfAudioCapturePorts )
    .add_property( "numberOfAudioPlaybackPorts", &AudioSignalFlow::numberOfAudioPlaybackPorts )
    .add_property( "numberOfCaptureChannels", &AudioSignalFlow::numberOfCaptureChannels )
    .add_property( "numberOfPlaybackChannels", &AudioSignalFlow::numberOfPlaybackChannels )
    .def( "audioCapturePortName", &AudioSignalFlow::audioCapturePortName, (arg("index")), return_internal_reference<>() )
    .def( "audioPlaybackPortName", &AudioSignalFlow::audioPlaybackPortName, (arg( "index" )), return_internal_reference<>() )
    ;
}

#endif

} // namespace rrl
} // namespace python
} // namespace visr
