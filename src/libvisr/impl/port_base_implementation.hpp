/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_IMPL_PORT_BASE_IMPLEMENTATIONHPP_INCLUDED
#define VISR_IMPL_PORT_BASE_IMPLEMENTATIONHPP_INCLUDED


#include "../port_base.hpp"
#include "../export_symbols.hpp"

#include <string>

namespace visr
{
namespace impl
{

// Forward declaration(s)
class ComponentImplementation;

class PortBaseImplementation
{
public:

  explicit VISR_CORE_LIBRARY_SYMBOL PortBaseImplementation( char const * name, ComponentImplementation * container, visr::PortBase::Direction direction );

  /**
   * Deleted default constructor.
   */
  PortBaseImplementation() = delete;

  /**
   * Deleted copy constructor to prevent copy construction of this and derived classes
   */
  PortBaseImplementation( PortBaseImplementation const & ) = delete;

  /**
   * Deleted move constructor to prevent move construction of this and derived classes
   */
  PortBaseImplementation( PortBaseImplementation && ) = delete;

  PortBaseImplementation & operator=( PortBaseImplementation const & ) = delete;

  PortBaseImplementation & operator=( PortBaseImplementation && ) = delete;

  /**
   * Destructor.
   * Ports are not intended to be used polymorphically, so the destructor is non-virtual.
   * @todo Check whether this still holds.
   */
  VISR_CORE_LIBRARY_SYMBOL  ~PortBaseImplementation();

  VISR_CORE_LIBRARY_SYMBOL  char const * name() const { return mName.c_str(); }

  VISR_CORE_LIBRARY_SYMBOL bool hasParent() const noexcept;

  VISR_CORE_LIBRARY_SYMBOL  ComponentImplementation const & parent() const;

  /**
   * Return the parent component of this port.
   * @throw std::logic if the the port has no parent.
   */
  VISR_CORE_LIBRARY_SYMBOL ComponentImplementation & parent();

  /*
   * Set a new parent to this port.
   * @param newParent the new parent component, use nullptr to unset the parent.
   */
  VISR_CORE_LIBRARY_SYMBOL void setParent( ComponentImplementation * newParent ) noexcept;

  /**
   * Remove the current parent, effectively making the 
   */
  VISR_CORE_LIBRARY_SYMBOL void removeParent() noexcept;

  VISR_CORE_LIBRARY_SYMBOL visr::PortBase::Direction direction() const { return mDirection; }

private:
  std::string const mName;

  ComponentImplementation * mParentComponent;

  visr::PortBase::Direction const mDirection;
};

} // namespace impl
} // namespace visr

#endif // #ifndef VISR_IMPL_PORT_BASE_IMPLEMENTATIONHPP_INCLUDED
