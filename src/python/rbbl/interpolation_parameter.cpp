/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librbbl/interpolation_parameter.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <ciso646>
#include <list>
#include <vector>
#include <sstream>

namespace visr
{
namespace rbbl
{
namespace python
{

namespace py = pybind11;

void exportInterpolationParameter( pybind11::module & m)
{

  py::class_<InterpolationParameter>( m, "InterpolationParameter" )
    .def( py::init<InterpolationParameter const &>(), py::arg( "rhs" ) )
    .def( py::init<InterpolationParameter::IdType, std::size_t>(), py::arg("id"), py::arg( "numberOfInterpolants" ) )
    .def( py::init<InterpolationParameter::IdType, InterpolationParameter::IndexContainer const &, InterpolationParameter::WeightContainer const &>(),
      py::arg( "id" ), py::arg( "indices" ), py::arg( "weights" ) )
    .def_property_readonly_static( "invalidIndex", []( InterpolationParameter const &){ return InterpolationParameter::cInvalidIndex; } )
    .def_property_readonly( "numberOfInterpolants", &InterpolationParameter::numberOfInterpolants )
    .def_property( "id", &InterpolationParameter::id, &InterpolationParameter::setId )
    .def( "index", &InterpolationParameter::index, py::arg("idx") )
    .def( "weight", &InterpolationParameter::weight, py::arg( "idx" ) )
    .def( "setIndex", &InterpolationParameter::setIndex, py::arg( "idx" ), py::arg("newIndex") )
    .def( "setWeight", &InterpolationParameter::setWeight, py::arg( "idx" ), py::arg( "newWeight" ) )
       .def_property( "indices", &InterpolationParameter::indices,
                  static_cast<void(InterpolationParameter::*)(InterpolationParameter::IndexContainer const &)>(&InterpolationParameter::setIndices) )
    .def_property( "weights", &InterpolationParameter::weights,
      static_cast<void(InterpolationParameter::*)(InterpolationParameter::WeightContainer const &)>(&InterpolationParameter::setWeights) )
    ;

  py::class_<InterpolationParameterSet>( m, "InterpolationParameterSet" )
  	.def( py::init( [](){ return new InterpolationParameterSet(); }  ) )
  	.def( py::init( []( std::list<InterpolationParameter> const & args )
  			{
  	  	  	  auto * inst = new InterpolationParameterSet( args.begin(), args.end() );
  	  	  	  return inst;
  			} ), py::arg( "interpolantList") )
  	.def( "__len__", &InterpolationParameterSet::size )
  	.def_property_readonly( "size", &InterpolationParameterSet::size )
  	.def( "__iter__", [](InterpolationParameterSet & self){ return py::make_iterator(self.begin(), self.end() ); } )
  	.def( "insert", [](InterpolationParameterSet & self, InterpolationParameter const & val ) { self.insert( val ); } )
  	.def( "__getitem__", [](InterpolationParameterSet & self, InterpolationParameter::IdType id )
  		  {
  	  	  	InterpolationParameter const cmp( id, 0 ); // Create a dummy element for searching
  	  	  	// C++14 would allow searching for a key without constructing an element.
  	  	  	InterpolationParameterSet::iterator const findIt = self.find( cmp );
  	  	  	return (findIt == self.end()) ? nullptr : &(*findIt);
  		  }, py::arg("index"), py::return_value_policy::reference, "Return an interpolation parameter for a given filter index" )
	;
}

} // namespace python
} // namepace pml
} // namespace visr
