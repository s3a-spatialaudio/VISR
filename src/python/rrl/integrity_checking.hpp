/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PYTHON_RRL_INTEGRITY_CHECKING_INCLUDED
#define VISR_PYTHON_RRL_INTEGRITY_CHECKING_INCLUDED

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#else
#include <boost/python.hpp>
#endif


namespace visr
{
namespace python
{
namespace rrl
{

#ifdef USE_PYBIND11

void exportIntegrityChecking( pybind11::module & m );

#else

void exportIntegrityChecking();

#endif

} // namepace rrl
} // namespace python
} // namespace visr

#endif // #ifndef VISR_PYTHON_RRL_INTEGRITY_CHECKING_INCLUDED
