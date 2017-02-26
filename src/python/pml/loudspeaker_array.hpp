/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PYTHON_PML_LOUDSPEAKER_ARRAY_HPP_INCLUDED
#define VISR_PYTHON_PML_LOUDSPEAKER_ARRAY_HPP_INCLUDED

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#endif

namespace visr
{
namespace python
{
namespace pml
{

#ifdef USE_PYBIND11
void exportLoudspeakerArray( pybind11::module & m );
#else
void exportLoudspeakerArray();
#endif

} // namepace pml
} // namespace python
} // namespace visr

#endif // #ifndef VISR_PYTHON_PML_LOUDSPEAKER_ARRAY_HPP_INCLUDED
