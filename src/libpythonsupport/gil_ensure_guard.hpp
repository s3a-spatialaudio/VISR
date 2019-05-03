/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBPYTHONSUPPORT_GIL_ENSURE_GUARD_HPP_INCLUDED
#define VISR_LIBPYTHONSUPPORT_GIL_ENSURE_GUARD_HPP_INCLUDED

#include "export_symbols.hpp"

#include <memory>

namespace visr
{
namespace pythonsupport
{

/**
 * RAII-style lock guard to ensure that the PyGILState_Ensure() / PyGILState_Realease() pair is executed in the
 * current scope.
 * This implementation differs from pybind11::gil_scoped_acquire in the sense that it can be called in an
 * arbitrary thread (that might not have a an initialized pybind11 local thread state.
 * @note Incidentally, this implementation is functionally identical with pybind11::gil_scoped_acquire if compiled for PyPy, but not when used with the standard CPython interface.
 *  
 */
class VISR_PYTHONSUPPORT_LIBRARY_SYMBOL GilEnsureGuard
{
public:
  /**
   * Constructor, ensures that the current thread is initialized to call Python C++ API and locks the Python global interpreter lock (GIL).
   */
  GilEnsureGuard();

  /**
   * Destructor, sets the thread state to the state it had before constructor had been called. Also
   * releases the GIL.
   */
  ~GilEnsureGuard();
private:
  /**
   * Forward declaration of private implementation object.
   */
  class Impl;

  /**
   * Private implementation object.
   * The primary reason for this object to avoid a Python interface dependency.
   */
  std::unique_ptr<Impl> mImpl;
};

} // namespace pythonsupport
} // namespace visr

#endif // #ifndef VISR_LIBPYTHONSUPPORT_GIL_ENSURE_GUARD_HPP_INCLUDED
