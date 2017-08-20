/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libril/parameter_factory.hpp>
#include <libril/parameter_type.hpp>
#include <libril/parameter_config_base.hpp>

#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace visr
{
namespace python
{
namespace visr
{

void exportParameterFactory( py::module& m )
{
  py::class_<ParameterFactory>( m, "ParameterFactory" )
    .def_property_readonly_static( "numberOfParameterTypes", [](py::object /*self*/){ return ParameterFactory::numberOfParameterTypes(); } )
    .def_static( "create", &ParameterFactory::create,
          py::arg("parameterType"), py::arg("parameterConfig") )
    .def_static( "typeExists", &ParameterFactory::typeExists, py::arg("typeId") )
    // TODO: Can we create types in Python?
    ;
}

} // namepace visr
} // namespace python
} // namespace visr
