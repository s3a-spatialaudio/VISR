/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "portaudio_interface.hpp"

namespace visr
{
namespace rrl
{

PortaudioInterface::PortaudioInterface( )
 : mCallback( nullptr )
{
}

PortaudioInterface::~PortaudioInterface()
{
}

/*virtual*/ bool 
PortaudioInterface::registerCallback( AudioCallback callback )
{
  mCallback = callback;
  return true;
}

/*virtual*/ bool 
PortaudioInterface::unregisterCallback( AudioCallback callback )
{
  if( mCallback == callback )
  {
    mCallback = nullptr;
    return true;
  }
  else
  {
    return false;
  }
}


} // namespace rrl
} // namespace visr
