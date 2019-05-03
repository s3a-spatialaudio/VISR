/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBPYTHONSUPPORT_INITIALISATION_GUARD_HPP_INCLUDED
#define VISR_LIBPYTHONSUPPORT_INITIALISATION_GUARD_HPP_INCLUDED

#include "export_symbols.hpp"

namespace visr
{

namespace pythonsupport
{

/**
 * Wrapper component that encapsulates arbitrary components (atomic or composite) to be instantiated and called from C++.
 */
class VISR_PYTHONSUPPORT_LIBRARY_SYMBOL InitialisationGuard
{
public:
  /**
   * Static function to enforce the initialisation of the Python library.
   * Subsequent calls have no effect.
   * @return Boolean flag whether the Python subsystem has been initialised.
   */
  static bool initialise();


  /**
   * Deleted default constructor to prohibit instantiation
   */
  InitialisationGuard() = delete;

private:

  /**
   * Internal implementation class that performs initialisation and finalisation.
   */
  class Internal;
};

} // namespace pythonsupport
} // namespace visr

#endif // #ifndef VISR_LIBPYTHONSUPPORT_INITIALISATION_GUARD_HPP_INCLUDED
