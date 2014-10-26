/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_PORTAUDIO_INTERFACE_HPP_INCLUDED
#define VISR_LIBRRL_PORTAUDIO_INTERFACE_HPP_INCLUDED

#include <libril/audio_interface.hpp>

namespace visr
{
namespace rrl
{



class PortaudioInterface: public ril::AudioInterface
{
public:
  using Base = ril::AudioInterface;

  PortaudioInterface();

  ~PortaudioInterface( );

  /*virtual*/ bool registerCallback( AudioCallback callback );

  /*virtual*/ bool unregisterCallback( AudioCallback audioCallback );
private:
  Base::AudioCallback mCallback;
};

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_PORTAUDIO_INTERFACE_HPP_INCLUDED