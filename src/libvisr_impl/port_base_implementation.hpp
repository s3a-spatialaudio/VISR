/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_IMPL_PORT_BASE_IMPLEMENTATIONHPP_INCLUDED
#define VISR_IMPL_PORT_BASE_IMPLEMENTATIONHPP_INCLUDED


#include <libril/port_base.hpp>
#include <libril/export_symbols.hpp>

#include <string>

namespace visr
{
namespace impl
{

// Forward declaration(s)
class ComponentImplementation;

class VISR_CORE_LIBRARY_SYMBOL PortBaseImplementation
{
public:

  explicit PortBaseImplementation( std::string const & name, ComponentImplementation * container, visr::PortBase::Direction direction );

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
   * Ports are not intended to be used polymorphically, so the desctructor is non-virtual.
   */
  ~PortBaseImplementation();

  std::string const & name() const { return mName; }

  bool hasParent() const noexcept;

  ComponentImplementation const & parent() const;

  /**
   * Return the parent component of this port.
   * @throw std::logic if the the port has no parent.
   */
  ComponentImplementation & parent();

  /*
   * Set a new parent to this port.
   * @param newParent the new parent component, use nullptr to unset the parent.
   */
  void setParent( ComponentImplementation * newParent ) noexcept;

  /**
   * Remove the current parent, effectively making the 
   */
  void removeParent() noexcept;

  visr::PortBase::Direction direction() const { return mDirection; }

private:
  std::string const mName;

  ComponentImplementation * mParentComponent;

  visr::PortBase::Direction const mDirection;
};

} // namespace impl
} // namespace visr

#endif // #ifndef VISR_IMPL_PORT_BASE_IMPLEMENTATIONHPP_INCLUDED
