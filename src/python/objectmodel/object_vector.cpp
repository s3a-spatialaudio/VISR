/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/object_vector_parser.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <sstream>

namespace py = pybind11;

namespace visr
{
namespace objectmodel
{
namespace python
{

void exportObjectVector( py::module & m )
{
  py::class_<ObjectVector>( m, "ObjectVector" )
    .def( py::init<>(), "Default constructor" )
    .def( py::init( []( std::vector<Object const *> vec )
     {
       ObjectVector * inst = new ObjectVector();
       for( Object const * obj : vec )
       {
         inst->insert( *obj );
       }
       return inst;
     }), py::arg("objects"), "Create an object vector out of a Pyhon list of objects." )
    .def( "__iter__", [](ObjectVector& ov){ return py::make_iterator(ov.begin(), ov.end() ); }, py::return_value_policy::reference_internal, "Return a Python iterator over all contained objects." )
    .def_property_readonly( "size", &ObjectVector::size, "Return the number of objects in the vector" )
    .def_property_readonly( "empty", &ObjectVector::empty, "Return whether the object vector is empty." )
    .def( "at", static_cast<Object&(ObjectVector::*)(ObjectId)>(&ObjectVector::at), py::return_value_policy::reference_internal, py::arg("id"), "Return a reference to an object with the given id." )
    .def( "__getitem__", static_cast<Object&(ObjectVector::*)(ObjectId)>(&ObjectVector::at), py::return_value_policy::reference_internal, py::arg("id"), "Return a reference to an object with the given id." )
    .def( "insert", static_cast<void(ObjectVector::*)(Object const&)>(&ObjectVector::insert), py::arg("obj"), "Set an object given id. If an object with that id already exists, it is replaced." )
    .def( "set", []( ObjectVector & ov, std::vector<Object const *> const & vec )
     {
       ov.clear();
       for( Object const * obj : vec )
       {
         ov.insert( *obj );
       }
     }, "Fill an object vector from a list of objects" )
    .def( "insert", [](ObjectVector & ov, std::vector<Object const *> const & vec )
     { for( Object const * obj : vec )
       {
         ov.insert( *obj );
       }
     }, "Insert a sequence of objects." )
    .def( "assign", &ObjectVector::assign, "Assign (copy) the contents from another object vector." )
    .def( "remove", &ObjectVector::remove, py::arg("objectId"), "Remove an object with a given id, throw an exception if no object exists with that id.")
    .def( "clear", &ObjectVector::clear, "Remove all objects from the vector." )
    .def( "fillFromJson", [](ObjectVector & vec, char const * jsonMsg ){ ObjectVectorParser::fillObjectVector( jsonMsg, vec ); }, py::arg("jsonString" ), "Fill the object vector from a JSON representation, clearing all other objects." )
    .def( "updateFromJson", [](ObjectVector & vec, char const * jsonMsg ){ ObjectVectorParser::updateObjectVector( jsonMsg, vec ); }, py::arg("jsonString" ), "Update the object vector from a JSON representation, keeping existing objects uness overwritten by new objects with the same id." )
    .def( "writeJson", [](ObjectVector const & vec ){ std::stringstream str; ObjectVectorParser::encodeObjectVector( vec, str ); return str.str();}, "Serialise the object vector to a JSON string." )
    ;
}

} // namespace python
} // namepace objectmodel
} // namespace visr
