/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libvisr/port_base.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include <ciso646>

namespace visr
{
namespace python
{
namespace visr
{

namespace py = pybind11;


void exportPort( py::module & m)
{
  /**
   * Define the common base class for audio and parameter ports.
   * Instantiation of this class is not intended, therefore no constructors are exposed.
   */
  py::class_<PortBase> port( m, "Port" );
  pybind11::enum_<PortBase::Direction>( port, "Direction" )
    .value( "Input", PortBase::Direction::Input )
    .value( "Output", PortBase::Direction::Output )
    .value( "All", PortBase::Direction::All )
    .def( pybind11::self | pybind11::self )
    .def( pybind11::self & pybind11::self )
    ;
}

} // namepace visr
} // namespace python
} // namespace visr
