/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBEFL_ALIGNMENT_HPP_INCLUDED
#define VISR_LIBEFL_ALIGNMENT_HPP_INCLUDED

#include <cstddef>
#include <ciso646> // for 'or' (should be obsolete in C++11, but MSVC 2013 still needs it)

/**
 * @file A collection of utility functions for dealing with
 * memory-aligned sequences of memory.
 * @note Within this project, alignment is typically given as a number
 * of elements, as opposed to a number of bytes.
 */

namespace visr
{
namespace efl
{

/**
 * Test whether a given number is an integer power of two (including 1,
 * but not 0)
 * @return <b>true</b> if it is a power of two, <b>false</b> otherwise.
 */
inline bool alignmentIsPowerOfTwo( std::size_t alignmentVal )
{
  return ((alignmentVal != 0) && ((alignmentVal & (~alignmentVal + 1)) == alignmentVal));
}

/**
 * Check whether a pointer meets a given alignment requirement.
 * That is, whether the address is a integer multiple of the element
 * size times the given alignment.
 * @tparam T The element type to which the given pointer points to.
 * @param ptr The pointer to be checked
 * @param alignment The requested alignment, given in number of
 * elements (instead of bytes). The alignment must be an integral
 * power of two, 1 and 0 are also allowed.
 * @return <b>true</b> if <tt>ptr</tt> meets the alignment
 * requirement, <b>false</b> otherwise.
 */
template<typename T>
bool checkAlignment( T const * ptr, std::size_t alignment )
{
  if( alignment == 0 )
  {
    return true;
  }
  std::size_t const actualAlignment = alignment * sizeof( T );
  // we assume that alignment is a power of two
  if( !alignmentIsPowerOfTwo(alignment) )
  {
    return false;
  }
  std::ptrdiff_t const bitMask = static_cast<std::ptrdiff_t>(actualAlignment - 1);
  std::ptrdiff_t ptrVal = reinterpret_cast<std::ptrdiff_t>(ptr); // todo: make this sound & safe
  return (ptrVal bitand bitMask) == 0;
}

/**
 * Compute the next number that is compatible with a given alignment
 * specification.
 * @param size A given number of elements
 * @param alignment The alignment requirement.
 * @return The smallest number that is larger or equal than <tt>size</tt> and
 * an integer multiple of <tt>alignment</tt>
 */
inline std::size_t nextAlignedSize( std::size_t size, std::size_t alignment )
{
  if( alignment == 0 )
  {
    return size;
  }
  std::size_t const rem = size % alignment;
  if( rem == 0 )
  {
    return size;
  }
  else
  {
    return (size / alignment + 1) * alignment;
  }
}

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_ALIGNMENT_HPP_INCLUDED
