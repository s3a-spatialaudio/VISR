/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "error_codes.hpp"

namespace visr
{
namespace efl
{

char const * errorMessage( const ErrorCode& error )
{
  switch( error )
  {
  case noError:
    return "No error";
  case alignmentError:
    return "Alignment error";
  case arithmeticError:
    return "Arithmetic error";
  // to be continued
  default:
    return "Unknown error code";
  }
}

} // namespace efl
} // namespace visr
