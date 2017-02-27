/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/port_base.hpp>

namespace visr
{

PortBase::PortBase( std::string const & name, Component & container, Direction direction )
 : mName( name )
 , mParentComponent( container )
 , mDirection( direction )
{
}

PortBase::~PortBase()
{
}

} // namespace visr
