/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libril/audio_port_base.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/audio_sample_type.hpp>
#include <libril/constants.hpp>

#include <libril/component.hpp>
#include <libril/signal_flow_context.hpp>

// Extend the interface beyond the audio ports visible in the C++ API
#include <libvisr_impl/audio_port_base_implementation.hpp>

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#else
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#include <boost/python/args.hpp>
#endif

namespace visr
{
namespace python
{
namespace visr
{

#ifdef USE_PYBIND11

namespace // unnamed
{

template<typename DataType>
pybind11::array_t<DataType> getInputBuffer( AudioInputT<DataType> & port )
{
  pybind11::buffer_info info ( const_cast<DataType*>(port.base()),
    sizeof(DataType),
    pybind11::format_descriptor<DataType>::format(),
    2 /* number of dimensions */,
    { port.width(), port.channelStrideSamples() },
    { sizeof( DataType ) * port.channelStrideSamples(), sizeof( DataType ) }
  );
  return pybind11::array_t<DataType>( info );
}

template<typename DataType>
pybind11::array_t<DataType> getOutputBuffer( AudioOutputT<DataType> & port )
{
  return pybind11::array_t<DataType>( pybind11::buffer_info( port.base(),
    sizeof( DataType ),
    pybind11::format_descriptor<DataType>::format(),
    2 /* number of dimensions */,
    { port.width(), port.channelStrideSamples() },
    { sizeof( DataType ) * port.channelStrideSamples(), sizeof( DataType ) }
  ) );
}

template<typename DataType>
pybind11::array_t<DataType> getInputChannel( AudioInputT<DataType> & port, std::size_t index )
{
  pybind11::buffer_info info( const_cast<DataType*>(port.at(index)),
    sizeof( DataType ),
    pybind11::format_descriptor<DataType>::format(),
    1 /* number of dimensions */,
    { port.channelStrideSamples() },
    { sizeof( DataType ) }
  );
  return pybind11::array_t<DataType>( info );
}

template<typename DataType>
pybind11::array_t<DataType> getOutputChannel( AudioOutputT<DataType> & port, std::size_t index )
{
  pybind11::buffer_info info( const_cast<DataType*>(port.at( index )),
    sizeof( DataType ),
    pybind11::format_descriptor<DataType>::format(),
    1 /* number of dimensions */,
    { port.channelStrideSamples() },
    { sizeof( DataType ) }
  );
  return pybind11::array_t<DataType>( info );
}

template<typename DataType>
void setOutputBuffer( pybind11::array_t<DataType> & matrix )
{
  pybind11::buffer_info info = matrix.request();
  // TODO: check compatibility of matrices.

  // TODO: Perform copying (accounting for matrix layout and alignment)
}

/// Bind an audio input of a given sample type to the specified name.
template<typename DataType >
void exportAudioInput( pybind11::module & m, char const * name )
{
  pybind11::class_<AudioInputT<DataType>, AudioPortBase >( m, name, pybind11::buffer_protocol() )
    .def( pybind11::init<char const*, Component &, std::size_t>(),
      pybind11::arg("name"), pybind11::arg("parent"), pybind11::arg("width") = 0 )
    .def_buffer( []( AudioInputT<DataType> &port ) -> pybind11::buffer_info
  {
    return pybind11::buffer_info( const_cast<DataType*>(port.base()),
      sizeof( DataType ),
      pybind11::format_descriptor<DataType>::format(),
      2 /* number of dimensions */,
      { port.width(), port.channelStrideSamples() },
      { sizeof( DataType ) * port.channelStrideSamples(), sizeof( DataType ) }
    );
  } )
  .def( "data", &getInputBuffer<DataType> )
  .def( "channel", &getInputChannel<DataType>, pybind11::arg("index") )
  .def( "__getitem__", &getInputChannel<DataType>, pybind11::arg( "index" ) )
    ;
}

/// Bind an audio output of a given sample type to the specified name.
template<typename DataType >
void exportAudioOutput( pybind11::module & m, char const * name )
{
  pybind11::class_<AudioOutputT<DataType>, AudioPortBase >( m, name, pybind11::buffer_protocol() )
    .def( pybind11::init<char const*, Component &, std::size_t>(),
      pybind11::arg( "name" ), pybind11::arg( "parent" ), pybind11::arg( "width" ) = 0 )
    .def_buffer( []( AudioOutputT<DataType> &port ) -> pybind11::buffer_info
  {
    return pybind11::buffer_info( const_cast<DataType*>(port.base()),
      sizeof( DataType ),
      pybind11::format_descriptor<DataType>::format(),
      2 /* number of dimensions */,
      { port.width(), port.channelStrideSamples() },
      { sizeof( DataType ) * port.channelStrideSamples(), sizeof( DataType ) }
    );
  } )

    .def( "data", &getOutputBuffer<DataType>, pybind11::return_value_policy::reference_internal )
    .def( "set", &setOutputBuffer<DataType> )
    // TODO: can we get an assign operator?
    .def( "channel", &getOutputChannel<DataType>, pybind11::return_value_policy::reference_internal, pybind11::arg( "index" ) )
    .def( "__getitem__", &getOutputChannel<DataType>, pybind11::return_value_policy::reference_internal, pybind11::arg( "index" ) )
    ;
}

} // unnamed namespace

void exportAudioPort( pybind11::module & m)
{
  pybind11::enum_<AudioSampleType::Id>(m, "AudioSampleType")
    .value( "floatId", AudioSampleType::floatId )
    .value( "doubleId", AudioSampleType::doubleId )
    .value( "longDoubleId", AudioSampleType::longDoubleId )
    .value( "uint8Id", AudioSampleType::uint8Id )
    .value( "int8Id", AudioSampleType::int8Id )
    .value( "uint16Id", AudioSampleType::uint16Id )
    .value( "int16Id", AudioSampleType::int16Id )
    .value( "uint32Id", AudioSampleType::uint32Id )
    .value( "int32Id", AudioSampleType::int32Id )
    ;

  /**
   * Define the common base class for audio ports.
   * Instantiation of this class is not intended, therefore no constructors are exposed.
   */
  pybind11::class_<AudioPortBase>( m, "AudioPortBase" )
    .def_property( "width", &AudioPortBase::width, &AudioPortBase::setWidth )
    .def_property_readonly( "alignmentSamples", &AudioPortBase::alignmentSamples )
    .def_property_readonly( "alignmentBytes", &AudioPortBase::alignmentBytes )
    .def_property_readonly( "channelStrideSamples", &AudioPortBase::channelStrideSamples )
    .def_property_readonly( "channelStrideBytes", &AudioPortBase::channelStrideBytes )
    .def_property_readonly( "sampleSize", &AudioPortBase::sampleSize )
    .def_property_readonly( "sampleType", &AudioPortBase::sampleType )
    .def_property_readonly( "initialised", []( AudioPortBase const & port ){ return port.implementation().initialised(); } )
    ;

  // Use the simple name for the default sample type (float)
  // Unfortunately pybind11 does not support aliases or  a second binding for the same C++ type
  exportAudioInput<float>( m, "AudioInput" );
  exportAudioInput<double>( m, "AudioInputDouble" );
  exportAudioInput<long double>( m, "AudioInputLongDouble" );
  exportAudioInput<int8_t>( m, "AudioInputInt8" );
  exportAudioInput<uint8_t>( m, "AudioInputUint8" );
  exportAudioInput<int16_t>( m, "AudioInputInt16" );
  exportAudioInput<uint16_t>( m, "AudioInputUint16" );
  exportAudioInput<int32_t>( m, "AudioInputInt32" );
  exportAudioInput<uint32_t>( m, "AudioInputUint32" );

  // Use the simple name for the default sample type (float)
  // Unfortunately pybind11 does not support aliases or  a second binding for the same C++ type
  exportAudioOutput<float>( m, "AudioOutput" );
  exportAudioOutput<double>( m, "AudioOutputDouble" );
  exportAudioOutput<long double>( m, "AudioOutputLongDouble" );
  exportAudioOutput<int8_t>( m, "AudioOutputInt8" );
  exportAudioOutput<uint8_t>( m, "AudioOutputUint8" );
  exportAudioOutput<int16_t>( m, "AudioOutputInt16" );
  exportAudioOutput<uint16_t>( m, "AudioOutputUint16" );
  exportAudioOutput<int32_t>( m, "AudioOutputInt32" );
  exportAudioOutput<uint32_t>( m, "AudioOutputUint32" );
}

#else
using namespace boost::python;

void exportAudioPort()
{
  PortBase::Direction (PortBase::*PortBaseDirection)() const = &PortBase::direction;

  class_<PortBase, boost::noncopyable>("PortBase", no_init )
    .def( init<std::string const &, Component &, PortBase::Direction>( args( "name", "parent" ) ) )
    .add_property( "name", make_function( &PortBase::name, return_internal_reference<>() ) )
    .add_property( "direction", &PortBase::direction )
//    .add_property( "parent", PortBaseDirection ) // Check how to select the const version
    .add_property( "parent", static_cast<PortBase::Direction( PortBase::*)() const>(&PortBase::direction) ) // Select the const method overload
  ;

  class_<AudioPortBase, bases<PortBase>, boost::noncopyable >("AudioPortBase", no_init )
    .def( init<char const*, Component *>( args( "name", "parent" ) ) )
    .add_property( "width", &AudioPortBase::width, &AudioPortBase::setWidth )
  ;
  
  class_<AudioInput, bases<AudioPortBase>, boost::noncopyable >( "AudioInput", no_init )
    .def( init<char const*, Component *>( args( "name", "parent" ) ) )
    .add_property( "width", &AudioPortBase::width, &AudioPortBase::setWidth )


}
#endif
} // namepace visr
} // namespace python
} // namespace visr
