/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "initialisation_guard.hpp"

#include <Python.h>

namespace visr
{

namespace pythonsupport
{

class InitialisationGuard::Internal
{
public:
  Internal()
  {
    Py_Initialize();
    mInitialised = true;
  }

  ~Internal()
  {
    mInitialised = false;
    Py_Finalize();
  }

  bool initialised() const
  {
    return mInitialised;
  }

private:
  bool mInitialised = false;
};

/*static*/ bool InitialisationGuard::initialise()
{
  static Internal sInternal;
  return sInternal.initialised();
}

} // namespace pythonsupport
} // namespace visr
