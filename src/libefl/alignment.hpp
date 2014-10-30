/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBEFL_ALIGNMENT_HPP_INCLUDED
#define VISR_LIBEFL_ALIGNMENT_HPP_INCLUDED

#include <cstddef>
#include <ciso646> // for 'or' (should be obsolete in C++11, but MSVC 2013 still needs it)

namespace visr
{
namespace efl
{

template<typename T>
bool checkAlignment( T const * ptr, std::size_t alignment )
{
  if( alignment == 0 )
  {
    return true;
  }
  std::size_t const actualAlignment = alignment * sizeof( T );
  // we assume that alignment is a power of two
  std::ptrdiff_t const bitMask = static_cast<std::ptrdiff_t>(actualAlignment - 1);
  std::ptrdiff_t ptrVal = reinterpret_cast<std::ptrdiff_t>(ptr); // todo: make this sound & safe
  return (ptrVal bitand bitMask) == 0;
}

inline std::size_t nextAlignedSize( std::size_t size, std::size_t alignment )
{
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
