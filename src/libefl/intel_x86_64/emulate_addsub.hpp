
/* Copyright (c) 2014-2019 Institute of Sound and Vibration Research,
   University of Southampton and VISR contributors --- All rights reserved. */
/* Copyright AudioScenic Ltd 2020 - All rights reserved */
/* @author Andreas Franck andreas.franck@audioscenic.com */

#include <immintrin.h>

namespace visr
{
namespace efl
{
namespace intel_x86_64
{
namespace detail
{

/**
 * Perform a simultaneous addition and subtraction on a SSE vector of floats.
 * Subtracts the even-indexed elements of \p a and \p b, and adds the odd-indexed elements.
 * This is a workaround for the missing _mm_addsub_ps intrinsic used in complex
 * multiplication, because this instruction does not exist before SSE3.
 * @note this function is not optimised for efficiency and used mainly for backward
 * compatibility. Calling functions should use conditional compilation to check for __SSE__,
 * and call _mm_addsub_ps directly if supported.
 */
inline __m128 emulateAddsubFloat( __m128 a, __m128 b )
{
  __m128 sum = _mm_add_ps( a, b );
  __m128 diff = _mm_sub_ps( a, b );
  sum = _mm_shuffle_ps( sum, sum, 0xDD );
  diff = _mm_shuffle_ps( diff, diff, 0x88 );
  __m128 res = _mm_movelh_ps( diff, sum );
  res =  _mm_shuffle_ps( res, res, 0xD8 );
  return res;
}

} // namespace detail
} // namespace intel_x86_64
} // namespace efl
} // namespace visr
