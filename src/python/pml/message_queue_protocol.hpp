/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PYTHON_PML_MESSAGE_QUEUE_PROTOCOL_HPP_INCLUDED
#define VISR_PYTHON_PML_MESSAGE_QUEUE_PROTOCOL_HPP_INCLUDED

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace pml
{

void exportMessageQueueProtocol( pybind11::module & m );

} // namepace pml
} // namespace python
} // namespace visr

#endif // #ifndef VISR_PYTHON_PML_MESSAGE_QUEUE_PROTOCOL_HPP_INCLUDED
