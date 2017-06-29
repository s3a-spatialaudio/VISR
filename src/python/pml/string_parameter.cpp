/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/string_parameter.hpp>
#include <libpml/string_parameter_config.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace visr
{

using pml::StringParameter;
using pml::StringParameterConfig;

namespace python
{
namespace pml
{

void exportStringParameter( pybind11::module & m)
{
  pybind11::class_<StringParameterConfig, ParameterConfigBase>( m, "StringParameterConfig" )
    .def( pybind11::init<std::size_t>(), pybind11::arg("maxLength")=0 )
    .def_property_readonly( "maxLength", &StringParameterConfig::maxLength )
  ;

  // Apparently it is not possible to add the base class std::string to the class binding 
  // (that results in a "Base class not known" error).
  // Therefore the contained string is accessed through the data property.
  pybind11::class_<StringParameter, ParameterBase>( m, "StringParameter" )
    .def_property_readonly_static( "staticType", []( pybind11::object /*self*/ ) { return StringParameter::staticType(); } )
    .def( pybind11::init<std::size_t>(), pybind11::arg("maxLength") )
    .def( pybind11::init<ParameterConfigBase const &>(), pybind11::arg("config") )
    .def( pybind11::init<StringParameterConfig const &>(), pybind11::arg("config") )
    .def( pybind11::init<std::string>(), pybind11::arg("initStr") )
    .def_property( "data", [](StringParameter const & self ){ return static_cast<std::string const &>(self); },
                   []( StringParameter & self, std::string const val ) { static_cast<std::string&>(self) = val; } )
    .def_property_readonly( "maxLength", &StringParameter::maxLength )
  ;
}

} // namepace pml
} // namespace python
} // namespace visr
