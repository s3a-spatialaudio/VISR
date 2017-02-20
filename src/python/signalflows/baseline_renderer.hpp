/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PYTHON_SIGNALFLOWS_BASELINE_RENDERER_HPP_INCLUDED
#define VISR_PYTHON_SIGNALFLOWS_BASELINE_RENDERER_HPP_INCLUDED

#ifdef USE_PYBIND11
#include <pybind11.h>
#endif

namespace visr
{
namespace python
{
namespace signalflows
{

#ifdef USE_PYBIND11
  void exportBaselineRenderer( pybind11::module& m );
#else
  void exportBaselineRenderer();
#endif

} // namepace signalflows
} // namespace python
} // namespace visr

#endif // #ifndef VISR_PYTHON_SIGNALFLOWS_BASELINE_RENDERER_HPP_INCLUDED
