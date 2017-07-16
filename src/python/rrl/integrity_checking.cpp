/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librrl/integrity_checking.hpp> 

#include <libril/component.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <sstream>
#include <tuple>
#include <vector>

namespace visr
{

namespace python
{
namespace rrl
{

void exportIntegrityChecking( pybind11::module & m )
{
  m.def( "checkConnectionIntegrity", []( Component const & component, bool hierarchical )
   { std::stringstream msg; bool res  = visr::rrl::checkConnectionIntegrity( component, hierarchical, msg ); return std::make_tuple(res, msg.str()); },
    pybind11::arg("component"), pybind11::arg("hierarchical") = true );
}

} // namespace rrl
} // namespace python
} // namespace visr
