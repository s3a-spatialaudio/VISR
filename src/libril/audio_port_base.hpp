/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_AUDIO_PORT_BASE_HPP_INCLUDED
#define VISR_LIBRIL_AUDIO_PORT_BASE_HPP_INCLUDED

#include "port_base.hpp"

#include "constants.hpp"

// #include <array>
#include <cstddef>
#include <exception>
#include <iterator>
#include <limits>
#include <string>
#include <vector>

#include <valarray>

namespace visr
{
namespace ril
{

// Forward declaration(s)
class Component;

class AudioPortBase: public PortBase
{
public:

  explicit AudioPortBase( std::string const & name, Component & container, Direction direction );

  explicit AudioPortBase( std::string const & name, Component& container, Direction direction, std::size_t width );

  ~AudioPortBase();

  /**
   * The type of signal indices.
   * @todo move to a central, unique definition
   */
  using SignalIndexType = std::size_t;

  /**
   * @note: Not required from user code?
   */
  static constexpr SignalIndexType cInvalidSignalIndex = std::numeric_limits<SignalIndexType>::max();

  /**
   * @note: Not required from user code?
   */
  static constexpr std::size_t cInvalidWidth = std::numeric_limits<SignalIndexType>::max();

  /**
   * Methods to be called by components 
   */
  //@{

  /**
   * @throw XXX if called while in initialised state.
   * @note Might be called from user code.
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
#if 0
  template< std::size_t vecLength >
  void assignCommunicationIndices( std::array<SignalIndexType, vecLength > const & indexVector )
  {
    assignCommunicationIndices( &indexVector[0], indexVector.size() );
  }

  void assignCommunicationIndices( SignalIndexType const * const val, std::size_t vecLength )
  {
    std::copy( val, val+vecLength, &mIndices[0] );
  }
#endif

  /**
   * @todo remove from header.
   * @todo remove from public interface
   * @todo consider changing the implementation to set the width according to the new index vector
   */
  void assignCommunicationIndices( std::vector<SignalIndexType> const & rhs )
  {
    if( rhs.size() != width() )
    {
      throw std::invalid_argument( "assignCommunicationIndices(): The size of the index vector does not match the port width." );
    }
    std::copy( rhs.begin(), rhs.end(), std::begin(mIndices) );
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

  std::valarray< SignalIndexType > mIndices;

  /**
   * Temporary container to hold the buffer addresses computed from the assigned indices.
   * As the location and the calling sequence for resolving the signal pointers is not decided yet, it is unclear whether 
   * this member is kept.
   */
  mutable std::valarray< SampleType* > mSignalPointers;
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_AUDIO_PORT_BASE_HPP_INCLUDED
