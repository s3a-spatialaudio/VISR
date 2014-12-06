/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_port.hpp"

#ifndef VISR_LIBRIL_AUDIO_PORT_ACCESS_PARENT_INLINE // otherwise it's already included
#include "audio_component.hpp"
#endif

namespace visr
{
namespace ril
{

/**
 * It appears that Visual Studio 2013 does not support this standard-compliant behaviour
 * for static const class members, i.e., it neither accepts nor compiles this explicit definition.
 /**
 * Provide a definition for the static const class members which is required by functions that pass
 * these members a reference.
 * @note The initialisation value is still set in the class definition (in case the members are used as
 * compile-time constants)
 * @note the support for constexpr (which is the better C++11 way to define such literal constants) 
 * differs between compilers, therefore there is an additional switch.
 */
//@{
#ifndef _MSC_VER

#if CPP_CONSTEXPR_SUPPORT
constexpr
#else
const /*static*/
#endif
std::size_t AudioPort::cInvalidWidth;

#if CPP_CONSTEXPR_SUPPORT
constexpr
#else
const  /*static*/
#endif
AudioPort::SignalIndexType AudioPort::cInvalidSignalIndex;

#endif // _MSC_VER
//@}

AudioPort::AudioPort( AudioComponent& container )
 : mParentComponent( container )
 , mWidth( cInvalidWidth )
{
}

AudioPort::AudioPort( AudioComponent& container, std::size_t width )
 : mParentComponent( container )
 , mWidth( cInvalidWidth ) // need to set it to some value before calling setWidth
{
  setWidth( width );
}

AudioPort::~AudioPort()
{}


void AudioPort::setWidth( std::size_t newWidth )
{
  if( initialised() )
  {
    throw std::logic_error( "AudioPort::setWidth must not be called while the system is initialised." );
  }
  mIndices.resize( newWidth, AudioPort::SignalIndexType(cInvalidSignalIndex) );
  mSignalPointers.resize( newWidth, nullptr );
  mWidth = newWidth;
}

#ifndef VISR_LIBRIL_AUDIO_PORT_ACCESS_PARENT_INLINE
bool AudioPort::initialised( ) const
{
  return mParentComponent.initialised( );
}

CommunicationArea<SampleType> & 
AudioPort::commArea( )
{
  return container( ).commArea( );
}

CommunicationArea<SampleType> const & 
AudioPort::commArea( ) const
{
  return container( ).commArea( );
}
#endif

} // namespace ril
} // namespace visr
