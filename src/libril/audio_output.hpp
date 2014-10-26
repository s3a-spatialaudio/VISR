/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_AUDIO_OUTPUT_HPP_INCLUDED
#define VISR_LIBRIL_AUDIO_OUTPUT_HPP_INCLUDED

#include "audio_port.hpp"

#include "communication_area.hpp"

namespace visr
{
namespace ril
{

// @todo: Rethink about public or protected inheritance (whether we want to use some kind of (static) polymorphism to access inputs and outpuits in a uniform way.
class AudioOutput: public AudioPort
{
public:
  explicit AudioOutput( char const * portName,
                        AudioComponent& container );

  ~AudioOutput();

  /**
   * Element-access interface to be used by derived audio components
   */
  //@{
  
  //@}
  SampleType * operator[]( std::size_t index )
  {
#ifndef NDEBUG
    if( !initialised( ) ) {
      throw std::logic_error( "Element access forbidden while the flow is not initialised" );
    }
#endif
    const SignalIndexType commIndex = indices( )[index];
    return commArea( )[commIndex];
  }

  SampleType * at( std::size_t index )
  {
    if( index >= width( ) ) {
      throw std::out_of_range( "Index exceeds the number of signals" );
    }
    return operator[]( index );
  }

  SampleType const * const * getVector( )
  {
#ifndef NDEBUG
    if( !initialised( ) ) {
      throw std::logic_error( "Element access forbidden while the flow is not initialised" );
    }
#endif
    SampleType * * ptrArray = signalPointers( );
    CommunicationArea<SampleType> & area = commArea( );
    for( std::size_t runIndex( 0 ); runIndex < width( ); ++runIndex ) {
      SampleType * tempPtr = area.at( indices( )[runIndex] );

      // TODO: sort out the const_cast issue later on!
      ptrArray[runIndex] = const_cast<visr::ril::SampleType*>(tempPtr);
    }
    return ptrArray;
  }

private:
};

}
}

#endif // #ifndef VISR_LIBRIL_AUDIO_OUTPUT_HPP_INCLUDED
