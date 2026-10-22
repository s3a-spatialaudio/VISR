/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "initialise_library.hpp"

#include <mutex>

#ifdef VISR_SYSTEM_PROCESSOR_x86_64
#include "intel_x86_64/initialise_library.hpp"
#endif

#ifdef VISR_SYSTEM_PROCESSOR_armv7l
#include "armv7l_neon_32bit/initialise_library.hpp"
#endif

namespace visr
{
namespace efl
{

namespace
{
// The backend-specific functions (un)register per-type function objects
// (Wrapper<T>::sPtr, std::function) that every vector function call reads.
// Without this lock, concurrent calls - e.g. several renderers constructed
// at once in a plugin host - race on those assignments (found with
// ThreadSanitizer).
std::mutex sRegistrationMutex;
}

bool initialiseLibrary( char const * processor /*= ""*/ )
{
  std::lock_guard<std::mutex> lock( sRegistrationMutex );
#ifdef VISR_SYSTEM_PROCESSOR_x86_64
  return intel_x86_64::initialiseLibrary( processor );
#endif
#ifdef VISR_SYSTEM_PROCESSOR_armv7l
  return armv7l_neon_32bit::initialiseLibrary( processor );
#endif
  return true;
}

bool uninitialiseLibrary()
{
  std::lock_guard<std::mutex> lock( sRegistrationMutex );
#ifdef VISR_SYSTEM_PROCESSOR_x86_64
  return intel_x86_64::uninitialiseLibrary();
#endif
#ifdef VISR_SYSTEM_PROCESSOR_armv7l
  return armv7l_neon_32bit::uninitialiseLibrary();
#endif
  return true;
}

} // namespace efl
} // namespace visr

