/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBEFL_ERROR_CODES_HPP_INCLUDED
#define VISR_LIBEFL_ERROR_CODES_HPP_INCLUDED

#include "export_symbols.hpp"

namespace visr
{
namespace efl
{

enum ErrorCode
{
  noError = 0,
  alignmentError = 1,
  arithmeticError = 2,
  logicError = 3
  // to be continued
};

/**
 * Return a textual description of the error state.
 */
VISR_EFL_LIBRARY_SYMBOL char const * errorMessage( const ErrorCode& error );

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_ERROR_CODES_HPP_INCLUDED
