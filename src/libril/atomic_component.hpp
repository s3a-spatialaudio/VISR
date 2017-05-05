/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_ATOMIC_COMPONENT_HPP_INCLUDED
#define VISR_ATOMIC_COMPONENT_HPP_INCLUDED

#include "component.hpp"

#include "export_symbols.hpp"

#include <cstddef>

namespace visr
{

/**
 * Base class for atomic components.
 * These components are at the lowest level in the hierarchy and implement runtime functionality as
 * C++ code.
 * Abstract base class, derived classes must override the virtual method process().
 */
class VISR_CORE_LIBRARY_SYMBOL AtomicComponent: public Component
{
public:
  /**
   * Constructor.
   * @param context a signal flow context structure containing general parameters, e.g., sampling rate and block size of computation.
   * @param name Null-terminated character string containing the name. Name must be unique within the containing composite component (if there is one).
   * @param parent A composite component to contain this atom, If it is a null pointer (the default), then this component is at the top level.
   */
  explicit /*VISR_CORE_LIBRARY_SYMBOL*/ AtomicComponent( SignalFlowContext const & context,
                                                         char const * name,
                                                         CompositeComponent * parent = nullptr );

  /**
   * Deleted copy constructor to avoid copying.
   */
  AtomicComponent( AtomicComponent const & ) = delete;

  /**
  * Deleted move constructor to avoid move construction.
  */
  AtomicComponent( AtomicComponent && ) = delete;

  /**
  * Destructor (virtual).
  * Atomic components are destined to be instantiated and managed polymorphically, thus requiring virtual destructors.
  */
  virtual  /*VISR_CORE_LIBRARY_SYMBOL*/ ~AtomicComponent() override;

  /**
   * Pure virtual process() function.
   * The overriding methods of base classes are called in regular intervals, each processing a fixed number (\p context.period())
   * number of samples.
   */
  virtual void process() = 0;

};

} // namespace visr

#endif // #ifndef VISR_ATOMIC_COMPONENT_HPP_INCLUDED
