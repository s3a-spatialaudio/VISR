/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/string_parameter.hpp>
#include <libpml/empty_parameter_config.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace visr
{

using pml::StringParameter;
using pml::EmptyParameterConfig;

namespace python
{
namespace pml
{

void exportStringParameter( pybind11::module & m)
{
  pybind11::class_<StringParameter, ParameterBase>( m, "StringParameter" )
    .def_property_readonly_static( "staticType", []( pybind11::object /*self*/ ) { return StringParameter::staticType(); } )
    .def( pybind11::init<std::size_t>(), pybind11::arg("maxLength") )
    .def( pybind11::init<ParameterConfigBase const &>(), pybind11::arg("config") )
    .def( pybind11::init<EmptyParameterConfig const &>(), pybind11::arg( "config" ) )
    .def( pybind11::init<std::string const &>(), pybind11::arg("initStr") )
    .def_property( "str", [](StringParameter const & self ){ return self.str(); },
                   static_cast<void(StringParameter::*)(std::string const&)>(&StringParameter::assign) )
    .def_property_readonly( "bytes", []( StringParameter const & self ) { return pybind11::bytes(self.str(), self.size()); },
      "Return a byte array of the parameter's content (not terminated by NULL characters)." )
    .def_property_readonly( "maxLength", &StringParameter::maxLength )
    .def_property_readonly( "empty", &StringParameter::empty )
    .def_property_readonly( "size", &StringParameter::size );
}

} // namepace pml
} // namespace python
} // namespace visr
