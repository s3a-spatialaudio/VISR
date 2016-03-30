/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_AUDIO_INPUT_HPP_INCLUDED
#define VISR_LIBRIL_AUDIO_INPUT_HPP_INCLUDED

#include "audio_port.hpp"
// not sure whether we want to expose the CommunicationArea mechanism to the ports 
// (and thus to the user code in the derived audio components)
#include "communication_area.hpp" 

namespace visr
{
namespace ril
{

// @todo: Rethink about public or protected inheritance (whether we want to use some kind of (static) polymorphism to access inputs and outpuits in a uniform way.
class AudioInput: public AudioPort
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
#ifndef NDEBUG
    if( !initialised() )
    {
      throw std::logic_error( "Element access forbidden while the flow is not initialised" );
    }
#endif
    const SignalIndexType commIndex = indices()[index];
    return commArea()[ commIndex ];
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
#ifndef NDEBUG
    if( !initialised( ) ) {
      throw std::logic_error( "Element access forbidden while the flow is not initialised" );
    }
#endif
    SampleType * * ptrArray = signalPointers( );
    CommunicationArea<SampleType> const & area =  commArea();
    for( std::size_t runIndex( 0 ); runIndex < width(); ++runIndex )
    {
      SampleType const * tempPtr = area.at( indices()[runIndex] );

      // TODO: sort out the const_cast issue later on!
      ptrArray[runIndex] = const_cast<visr::ril::SampleType*>(tempPtr);
    }
    return ptrArray;
  }

  //@}

private:
};

}
}

#endif // #ifndef VISR_LIBRIL_AUDIO_INPUT_HPP_INCLUDED
