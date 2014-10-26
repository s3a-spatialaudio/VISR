/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBEFL_ERROR_CODES_HPP_INCLUDED
#define VISR_LIBEFL_ERROR_CODES_HPP_INCLUDED

namespace visr
{
namespace efl
{

enum ErrorCode
{
  noError = 0,
  alignmentError = 1,
  arithmeticError = 2
  // to be continued
};

/**
 * Return a textual description of the error state.
 */
char const * errorMessage( const ErrorCode& error );

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_ERROR_CODES_HPP_INCLUDED
