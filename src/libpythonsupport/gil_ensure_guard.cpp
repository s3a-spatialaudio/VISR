/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "gil_ensure_guard.hpp"

// Use the pybind11 header to include Python.h. This handles the quirky workarounds
// needed to include the raw Python include. 
#include <pybind11/pybind11.h>

namespace visr
{
namespace pythonsupport
{

class GilEnsureGuard::Impl
{
public:
  Impl()
  {
    // This would create a fatal error (abortion?) on failure.
    mState = PyGILState_Ensure();
  }

  ~Impl()
  {
    PyGILState_Release( mState );
  }
private:
  PyGILState_STATE mState;
};
 
GilEnsureGuard::GilEnsureGuard()
    : mImpl( new Impl() )
{}

GilEnsureGuard::~GilEnsureGuard() = default;

} // namespace pythonsupport
} // namespace visr
