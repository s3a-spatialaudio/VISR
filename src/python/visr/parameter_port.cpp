/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libril/component.hpp>
#include <libril/parameter_config_base.hpp>
#include <libril/parameter_input.hpp>
#include <libril/parameter_output.hpp>
#include <libril/parameter_port_base.hpp>

#include <libvisr_impl/polymorphic_parameter_input.hpp>


#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>

#else
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#include <boost/python/args.hpp>
#endif

#include <ciso646>

namespace visr
{
namespace python
{
namespace visr
{

#ifdef USE_PYBIND11

void exportParameterPort( pybind11::module & m)
{
  /**
   * Define the common base class for parameter ports.
   * Instantiation of this class is not intended, therefore no constructors are exposed.
   */
  pybind11::class_<ParameterPortBase>( m, "ParameterPortBase" )
    .def_property_readonly( "parameterType", &ParameterPortBase::parameterType )
    .def_property_readonly( "protocolType", &ParameterPortBase::protocolType )
    .def_property( "parameterConfig", &ParameterPortBase::parameterConfig, &ParameterPortBase::setParameterConfig )
    .def( "hasParameterConfig", &ParameterPortBase::setParameterConfig, pybind11::arg("parameterConfig"))
    ;

  pybind11::class_<ParameterInputBase, ParameterPortBase>( m, "ParameterInput" )
    .def( "protocolInput", &ParameterInputBase::protocolInput )
    ;

  pybind11::class_<ParameterOutputBase, ParameterPortBase>( m, "ParameterOutput" )
    .def( "protocolOutput", &ParameterOutputBase::protocolOutput )
    ;

  pybind11::class_<PolymorphicParameterInput, ParameterInputBase>( m, "PolyParameterInput" )
    .def( pybind11::init<char const *, Component &, ParameterType const &, CommunicationProtocolType const &, ParameterConfigBase const &>(),
      pybind11::arg("name"), pybind11::arg("parent"), pybind11::arg( "parameterType" ),
      pybind11::arg("protocolType"), pybind11::arg("parameterConfig" ) )
    .def( pybind11::init<char const *, Component &, ParameterType const &, CommunicationProtocolType const &>(),
      pybind11::arg( "name" ), pybind11::arg( "parent" ), pybind11::arg( "parameterType" ),
      pybind11::arg( "protocolType" ) )
    ;
}
#endif
} // namepace visr
} // namespace python
} // namespace visr
