/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librrl/integrity_checking.hpp> 

#include <libril/component.hpp>

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#else
#include <boost/python.hpp>
#include <boost/python/args.hpp>
#include <boost/noncopyable.hpp>
#endif

#include <sstream>
#include <tuple>
#include <vector>

namespace visr
{

namespace python
{
namespace rrl
{

#ifdef USE_PYBIND11

void exportIntegrityChecking( pybind11::module & m )
{
  m.def( "checkConnectionIntegrity", []( Component const & component, bool hierarchical )
   { std::stringstream msg; bool res  = visr::rrl::checkConnectionIntegrity( component, hierarchical, msg ); return std::make_tuple(res, msg.str()); },
    pybind11::arg("component"), pybind11::arg("hierarchical") = true );
}


#else

using namespace boost::python;

#error "rrl integrity checking Python bindings not implemented for boost::Python"

#endif

} // namespace rrl
} // namespace python
} // namespace visr
