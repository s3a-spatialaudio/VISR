/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_AUDIO_INPUT_HPP_INCLUDED
#define VISR_AUDIO_INPUT_HPP_INCLUDED

#include "audio_port_base.hpp"

namespace visr
{

// @todo: Rethink about public or protected inheritance (whether we want to use some kind of (static) polymorphism to access inputs and outputs in a uniform way.
class AudioInput: public AudioPortBase
{
public:
  explicit AudioInput( char const * portName,
                       Component& container );

  ~AudioInput();

  /**
   * Element-access interface to be used by derived audio components
   */
  //@{
  SampleType const * operator[]( std::size_t index ) const
  {
    const SignalIndexType commIndex = indices()[index];
    return mAudioBasePtr + commIndex * mAudioChannelStride;
  }

  SampleType const * at( std::size_t index ) const
  {
    if( index >= width() ) 
    {
      throw std::out_of_range( "Index exceeds the number of signals" );
    }
    return operator[]( index );
  }

  SampleType const * const * getVector()
  {
    SampleType * * ptrArray = signalPointers( );
    for( std::size_t runIndex( 0 ); runIndex < width(); ++runIndex )
    {
      // TODO: sort out the const_cast issue later on!
      ptrArray[runIndex] = const_cast<SampleType*>(operator[](runIndex));
    }
    return ptrArray;
  }
  //@}

private:
};

} // namespace visr

#endif // #ifndef VISR_AUDIO_INPUT_HPP_INCLUDED
