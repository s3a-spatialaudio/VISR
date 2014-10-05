/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_AUDIO_INTERFACE_HPP_INCLUDED
#define VISR_LIBRIL_AUDIO_INTERFACE_HPP_INCLUDED

namespace visr
{
namespace ril
{


/**
 * Abstract interface for various audio interfaces.
 */
class AudioInterface
{
public:
  // Preliminary definition of sample types
  using ExternalSampleType = float;
  using InternalSampleType = float;

  typedef int( *audioCallback )( void*,
                                 ExternalSampleType const * const *,
                                 ExternalSampleType * const *,
                                 int );

  virtual bool registerCallback( audioCallback ) = 0;

  virtual bool unregisterCallback( audioCallback ) = 0;

};

}
}

#endif // #ifndef VISR_LIBRIL_AUDIO_INTERFACE_HPP_INCLUDED