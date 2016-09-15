/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_AUDIO_PORT_HPP_INCLUDED
#define VISR_LIBRIL_AUDIO_PORT_HPP_INCLUDED

#include "port_base.hpp"

#include "constants.hpp"

#include <array>
#include <cstddef>
#include <exception>
#include <iterator>
#if CPP_CONSTEXPR_SUPPORT
#include <limits>
#else
#include <climits> // to work around the current limitations of MSVC regarding constexpr
#endif
#include <string>

// Temporary solution to get rid of the annoying MSVC unsafe argument warnings when using STL algorithms on std::valarrays
// #define AUDIOPORT_USE_VECTOR_FOR_INDICES

#ifdef AUDIOPORT_USE_VECTOR_FOR_INDICES
#include <vector>
#else
#include <valarray>
#endif

namespace visr
{
namespace ril
{

// Forward declaration(s)
class Component;

class AudioPort: public PortBase
{
public:

  explicit AudioPort( std::string const & name, Component & container, Direction direction );

  explicit AudioPort( std::string const & name, Component& container, Direction direction, std::size_t width );

  ~AudioPort();

  /**
   * The type of signal indices.
   * @todo move to a central, unique definition
   */
  using SignalIndexType = std::size_t;

  const static SignalIndexType cInvalidSignalIndex = UINT64_MAX;


  const static std::size_t cInvalidWidth = UINT64_MAX;

  /**
   * Methods to be called by components 
   */
  //@{

  /**
   * @throw XXX if called while in initialised state.
   */
  void setWidth( std::size_t newWidth );

  std::size_t width() const { return mWidth; }

  //@}

  void setAudioChannelStride(std::size_t stride)
  {
	mAudioChannelStride = stride;
  }

  void setAudioBasePointer(ril::SampleType * const base) 
  {
	mAudioBasePtr = base;
  }

protected:
  ril::SampleType * mAudioBasePtr;

  std::size_t mAudioChannelStride;

public: // Temporary solution to make the indices visible to the runtime infrastructure.

  SignalIndexType const * indices() const { return &mIndices[0]; }

  /**
   * Methods to be called by derived audio port classes classes
   */
  //@{

  //@}

  template< std::size_t vecLength >
  void assignCommunicationIndices( std::array<SignalIndexType, vecLength > const & indexVector )
  {
    assignCommunicationIndices( &indexVector[0], indexVector.size() );
  }

  void assignCommunicationIndices( SignalIndexType const * const val, std::size_t vecLength )
  {
#ifdef AUDIOPORT_USE_VECTOR_FOR_INDICES
    std::copy( val, val + vecLength, mIndices.begin( ) );
#else
    std::copy( val, val+vecLength, &mIndices[0] );
#endif
  }

  template<typename IteratorType>
  void assignCommunicationIndices( IteratorType begin, IteratorType end )
  {
    // calculating the distance might be linear in the length of the sequence, but this is proportional to 
    // the running time of the copy call below and thus acceptable.
    typename std::iterator_traits<IteratorType>::difference_type numElements = std::distance( begin, end );
    if( numElements != mWidth )
    {
      throw std::invalid_argument( "AudioPort: The length of the sequence passed to assignCommunicationIndices() must match the width of the port." );
    }
    if( numElements > 0 ) // MSVC triggers an debug assertion for &mIndices[0] if the size is zero.
    {
      std::copy( begin, end, &mIndices[0] );
    }
  }

  SampleType * * signalPointers() { return &mSignalPointers[0]; }
private:
  std::size_t mWidth;

#ifdef AUDIOPORT_USE_VECTOR_FOR_INDICES
  std::vector< SignalIndexType > mIndices;
#else
  std::valarray< SignalIndexType > mIndices;
#endif

  /**
   * Temporary container to hold the buffer addresses computed from the assigned indices.
   * As the location and the calling sequence for resolving the signal pointers is not decided yet, it is unclear whether 
   * this member is kept.
   */
#ifdef AUDIOPORT_USE_VECTOR_FOR_INDICES
  mutable std::vector< SampleType* > mSignalPointers;
#else
  mutable std::valarray< SampleType* > mSignalPointers;
#endif
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_AUDIO_PORT_HPP_INCLUDED
