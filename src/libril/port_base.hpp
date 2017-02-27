/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PORT_BASE_HPP_INCLUDED
#define VISR_PORT_BASE_HPP_INCLUDED

#include <string>

namespace visr
{

// Forward declaration(s)
class Component;

class PortBase
{
public:
  enum class Direction
  {
    Input,
    Output
  };

  explicit PortBase( std::string const & name, Component & container, Direction direction );

  /**
   * Deleted default constructor.
   */
  PortBase() = delete;

  /**
   * Deleted copy constructor to prevent copy construction of this and derived classes
   */
  PortBase( PortBase const & ) = delete;

  /**
   * Deleted move constructor to prevent move construction of this and derived classes
   */
  PortBase( PortBase && ) = delete;

  PortBase & operator=( PortBase const & ) = delete;

  PortBase & operator=( PortBase && ) = delete;

  /**
   * Destructor.
   * Ports are not intended to be used polymorphically, so the desctructor is non-virtual.
   */
  ~PortBase();

  std::string const & name() const { return mName; }

  Component const & parent() const { return mParentComponent; }

  Component & parent() { return mParentComponent; }

  Direction direction() const { return mDirection; }

private:
  std::string const mName;

  Component & mParentComponent;

  Direction const mDirection;
};

} // namespace visr

#endif // #ifndef VISR_PORT_BASE_HPP_INCLUDED
