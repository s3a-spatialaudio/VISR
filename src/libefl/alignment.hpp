/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBEFL_ALIGNMENT_HPP_INCLUDED
#define VISR_LIBEFL_ALIGNMENT_HPP_INCLUDED

#include <cstddef>

namespace visr
{
namespace efl
{

template<typename T>
bool checkAlignment( T const *, std::size_t alignment )
{
  // fake for the moment
  return true;
}

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_ALIGNMENT_HPP_INCLUDED
