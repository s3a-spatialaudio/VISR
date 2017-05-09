/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "initialisation_guard.hpp"

// Use the pybind11 header to include Python.h (instead of doing this directly)
// This is to overcome the pecularities of Python.h on Windows, 
// which enforces linking to a debug version of the python library.
// The pybind11 header already contains the magic to circumvent this.
#include <pybind11/pybind11.h>

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
    PyEval_InitThreads();
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
