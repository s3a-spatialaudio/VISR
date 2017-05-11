/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PYTHON_PML_LOUDSPEAKER_ARRAY_HPP_INCLUDED
#define VISR_PYTHON_PML_LOUDSPEAKER_ARRAY_HPP_INCLUDED

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace pml
{

void exportLoudspeakerArray( pybind11::module & m );

} // namepace pml
} // namespace python
} // namespace visr

#endif // #ifndef VISR_PYTHON_PML_LOUDSPEAKER_ARRAY_HPP_INCLUDED
