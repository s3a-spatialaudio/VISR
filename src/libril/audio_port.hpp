/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_AUDIO_PORT_HPP_INCLUDED
#define VISR_LIBRIL_AUDIO_PORT_HPP_INCLUDED

// Whether to use inline functions to access the containing components.
// Provide facility to switch off to break suspected dependency cycle.
// #define VISR_LIBRIL_AUDIO_PORT_ACCESS_PARENT_INLINE

// Check how to make warning C4996 disappear
#define _CRT_SECURE_NO_WARNINGS 1

/**
 * Define whether constexpr keyword is supported by the compiler.
 * @note Maybe this constant should be set the build system.
 * @todo Clean up after all compilers (i.e., MSVC) support constexpr correctly.
 */
#define CPP_CONSTEXPR_SUPPORT 0

#include "constants.hpp"
#ifdef VISR_LIBRIL_AUDIO_PORT_ACCESS_PARENT_INLINE
// TODO: Rethink whether we want this include here. 
// (needed due to inline inline implementations which directly call member functions of their parent component
#include "audio_component.hpp"
#endif

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
#define AUDIOPORT_USE_VECTOR_FOR_INDICES

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
// Removed due to inclusion of header (see above)
#ifndef VISR_LIBRIL_AUDIO_PORT_ACCESS_PARENT_INLINE
class AudioSignalFlow;
#endif
// I'm not sure yet whether the components should know about the CommunicationArea mechanism,
// or whether they should use just plain pointer arrays.
template< typename Type > class CommunicationArea;

class AudioPort
{
public:
  friend class AudioSignalFlow;

  /**
   * The type of signal indices.
   * @todo move to a central, unique definition
   */
  using SignalIndexType = std::size_t;

#if CPP_CONSTEXPR_SUPPORT
  constexpr static std::size_t cInvalidWidth = std::numeric_limits<std::size_t>::max();
#else
  const static std::size_t cInvalidWidth = UINT_MAX;
#endif

#if CPP_CONSTEXPR_SUPPORT
  constexpr static SignalIndexType cInvalidSignalIndex = std::numeric_limits<SignalIndexType>::max();
#else
  const static SignalIndexType cInvalidSignalIndex = UINT_MAX;
#endif

  explicit AudioPort( Component& container );

  explicit AudioPort( Component& container, std::size_t width );

  ~AudioPort();

  bool initialised() const
#ifdef VISR_LIBRIL_AUDIO_PORT_ACCESS_PARENT_INLINE
  {
 return mParentComponent.initialised(); }
#else
    ;
#endif

  /**
   * Methods to be called by components 
   */
  //@{

  /**
   * @throw XXX if called while in initialiased state.
   */
  void setWidth( std::size_t newWidth );

  std::size_t width() const { return mWidth; }

  //@}

protected:
  Component & container() { return mParentComponent; }

  Component const & container( ) const { return mParentComponent; }

  /**
   * @todo make 'em inline after breaking the (suspected) cyclic dependency.
   */
  //@{
  CommunicationArea<SampleType> & commArea()
#ifdef VISR_LIBRIL_AUDIO_PORT_ACCESS_PARENT_INLINE
  { return container().commArea(); }
#else
    ;
#endif
  CommunicationArea<SampleType> const & commArea() const
#ifdef VISR_LIBRIL_AUDIO_PORT_ACCESS_PARENT_INLINE
  { return container().commArea(); }
#else
    ;
#endif
  //@}

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
    // std::copy( begin, end, &mIndices[0] );
    std::copy( begin, end, mIndices.begin() ); // C++11 feature std::valarray<T>::begin(), but not featured either in VC 2013 and GCC 4.8 at this time.
  }

  SampleType * * signalPointers() { return &mSignalPointers[0]; }
private:

  Component& mParentComponent;

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

#endif // #ifndef VISR_LIBRIL_AUDIO_COMPONENT_HPP_INCLUDED
