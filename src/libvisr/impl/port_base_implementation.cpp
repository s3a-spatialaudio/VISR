/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "port_base_implementation.hpp"

#include <ciso646>
#include <stdexcept>

namespace visr
{
namespace impl
{

PortBaseImplementation::PortBaseImplementation( char const * name, ComponentImplementation * container, visr::PortBase::Direction direction )
 : mName( name )
 , mParentComponent( container )
 , mDirection( direction )
{
}

PortBaseImplementation::~PortBaseImplementation()
{
}

bool PortBaseImplementation::hasParent() const noexcept
{
  return mParentComponent != nullptr;
}

ComponentImplementation const & PortBaseImplementation::parent() const
{
  if( not hasParent() )
  {
    throw std::logic_error( "PortBaseImplementation::parent(): no parent set." );
  }
  return *mParentComponent;
}

ComponentImplementation & PortBaseImplementation::parent()
{
  if( not hasParent() )
  {
    throw std::logic_error( "PortBaseImplementation::parent(): no parent set." );
  }
  return *mParentComponent;
}

void PortBaseImplementation::setParent( ComponentImplementation * newParent ) noexcept
{
  mParentComponent = newParent;
}

void PortBaseImplementation::removeParent() noexcept
{
  setParent( nullptr );
}

} // namespace impl
} // namespace visr
