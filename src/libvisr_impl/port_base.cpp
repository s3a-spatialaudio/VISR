/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "port_base.hpp"


namespace visr
{
namespace ril
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

} // namespace ril
} // namespace visr
