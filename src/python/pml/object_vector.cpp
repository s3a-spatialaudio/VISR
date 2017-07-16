/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libpml/object_vector.hpp>

#include <pybind11/pybind11.h>

namespace visr
{

using pml::ObjectVector;

namespace python
{
namespace pml
{

void exportObjectVector( pybind11::module & m)
{
  pybind11::class_<visr::pml::ObjectVector, ParameterBase, objectmodel::ObjectVector>( m, "ObjectVector" )
    .def_property_readonly_static( "staticType", [](pybind11::object /*self*/) {return ObjectVector::staticType(); } )
    // .def( pybind11::init<>() ) // Do we need a default constructor
    .def( pybind11::init<visr::pml::ObjectVector const &>(), "Copy construction of an object vector." )
    .def( pybind11::init<ParameterConfigBase const &>(), "Construction from a parameter config base object. The dynamic type must be EmptyParameterConfig" )
    ;
}

} // namepace pml
} // namespace python
} // namespace visr
