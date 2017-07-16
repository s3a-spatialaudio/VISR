/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librrl/audio_signal_flow.hpp> 

#include <libril/component.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <vector>

namespace visr
{

using rrl::AudioSignalFlow;

namespace python
{
namespace rrl
{

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
  {
    throw std::invalid_argument( "AudioSignalFlow::process(): Dimension 1 input of the input matrix does not match the number of capture channels." );
  }
  if( input.shape(1) != flow.period() )
  {
    throw std::invalid_argument( "AudioSignalFlow::process(): Dimension 0 input of the input matrix does not match the block size of the signal flow." );
  }

  pybind11::array outputSignal( pybind11::dtype::of<SampleType>(),
  { flow.numberOfPlaybackChannels(), flow.period() },
  { sizeof( SampleType )*flow.period(), sizeof( SampleType ) } // TODO: Take care of alignment
                              );
  std::size_t const outChannelStride = outputSignal.strides(0) / sizeof(DataType);
  std::size_t const outSampleStride = outputSignal.strides( 1 ) / sizeof( DataType );

  try
  {
    flow.process( static_cast<SampleType const *>(input.data()),
                  input.strides(0)/sizeof(SampleType),
                  input.strides(1)/sizeof(SampleType),
                  static_cast<SampleType *>(outputSignal.mutable_data()),
                  outChannelStride,
                  outSampleStride );
  }
  catch( std::exception const & ex )
  {
    // For the time being we are using std::invalid_argument because it is recognised by pybind11 and translated to a proper Python exeption
    // todo: register a more fitting exception, e.g., std::runtime_error
    throw std::invalid_argument( detail::composeMessageString( "Exception while execution signal flow:", ex.what() ) );
  }
  return outputSignal;
}

void exportAudioSignalFlow( pybind11::module & m )
{
  pybind11::class_<AudioSignalFlow>( m, "AudioSignalFlow" )
   .def( pybind11::init<visr::Component&>() )
   .def_property_readonly( "numberOfAudioCapturePorts", &AudioSignalFlow::numberOfAudioCapturePorts )
   .def_property_readonly( "numberOfAudioPlaybackPorts", &AudioSignalFlow::numberOfAudioPlaybackPorts )
   .def_property_readonly( "numberOfCaptureChannels", &AudioSignalFlow::numberOfCaptureChannels )
   .def_property_readonly( "numberOfPlaybackChannels", &AudioSignalFlow::numberOfPlaybackChannels )
   .def_property_readonly( "numberParameterReceivePorts", &AudioSignalFlow::numberExternalParameterReceivePorts )
   .def_property_readonly( "numberParameterSendPorts", &AudioSignalFlow::numberExternalParameterSendPorts )
   .def( "parameterReceivePort", &AudioSignalFlow::externalParameterReceivePort, pybind11::arg("portName"), pybind11::return_value_policy::reference )
   .def( "parameterSendPort", &AudioSignalFlow::externalParameterSendPort, pybind11::arg("portName"), pybind11::return_value_policy::reference )
   .def( "parameterReceivePorts", &AudioSignalFlow::externalParameterReceiveEndpoints, pybind11::return_value_policy::reference )
   .def( "parameterSendPorts", &AudioSignalFlow::externalParameterSendEndpoints )
   .def( "audioCapturePortName", &AudioSignalFlow::audioCapturePortName, pybind11::arg("index"), pybind11::return_value_policy::reference )
   .def( "audioPlaybackPortName", &AudioSignalFlow::audioPlaybackPortName, pybind11::arg( "index" ), pybind11::return_value_policy::reference )
   .def( "process", [](visr::rrl::AudioSignalFlow & flow, pybind11::array const & input) /*-> pybind11::array_t<SampleType>*/ { return wrapProcess( flow, input );}, pybind11::return_value_policy::take_ownership )
  ;
}

} // namespace rrl
} // namespace python
} // namespace visr
