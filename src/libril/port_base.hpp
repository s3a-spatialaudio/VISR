/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_PORT_BASE_HPP_INCLUDED
#define VISR_LIBRIL_PORT_BASE_HPP_INCLUDED

#include <string>

namespace visr
{
namespace ril
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
   * Destructor.
   * Ports are not intended to be used polymoprphically, so the desctructor is non-virtual.
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

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_PORT_BASE_HPP_INCLUDED
