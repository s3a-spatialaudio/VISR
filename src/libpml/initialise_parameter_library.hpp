/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_INITIALISE_PARAMETER_LIBRARY_HPP_INCLUDED
#define VISR_PML_INITIALISE_PARAMETER_LIBRARY_HPP_INCLUDED

#include "export_symbols.hpp"

namespace visr
{
namespace pml
{

/**
 * Register all contained parameter types and communication protocols in the respective factories.
 */
VISR_PML_LIBRARY_SYMBOL void initialiseParameterLibrary();

} // namespace pml
} // namespace visr


#endif // VISR_PML_INITIALISE_PARAMETER_LIBRARY_HPP_INCLUDED
