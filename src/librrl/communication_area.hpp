/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_RRL_COMMUNICATION_AREA_HPP_INCLUDED
#define VISR_RRL_COMMUNICATION_AREA_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libefl/aligned_array.hpp>

#include <stdexcept>
#include <valarray>

namespace visr
{
namespace rrl
{


class VISR_RRL_LIBRARY_SYMBOL AudioSignalPool
{
public:
  explicit AudioSignalPool( std::size_t size, std::size_t alignment );

  ~AudioSignalPool();

  char * basePointer() { return mPool.data(); }

  char const * basePointer() const { return mPool.data(); }

private:
  efl::AlignedArray<char> mPool;
};

/**
 *
 */
template<typename SampleType>
class VISR_RRL_LIBRARY_SYMBOL CommunicationArea
{
public:
  /**
   * Constructor.
   * @param numberOfSignals
   * @param signalLength
   * @param alignmentElements [optional]
   */
  explicit CommunicationArea( std::size_t numberOfSignals, std::size_t signalLength,
                              std::size_t alignmentElements /* = 0 */ );
  /** Destructor */
  ~CommunicationArea();

  std::size_t numberOfSignals() const { return mNumberOfSignals; }

  std::size_t signalLength() const { return mSignalLength; }

  std::size_t signalStride() const { return mSignalStride; }

  SampleType * data() { return mStorage.data(); }

  SampleType const * data( ) const { return mStorage.data( ); }

  SampleType * operator[]( std::size_t index )
  {
    return data() + mSignalStride * index;
  }

  SampleType const * operator[]( std::size_t index ) const
  {
    return data( ) + mSignalStride * index;
  }

  SampleType * at( std::size_t index )
  {
    if( index >= mNumberOfSignals )
    {
      throw std::out_of_range( "Index exceeds the number of signals" );
    }
    return operator[]( index );
  }

  SampleType const * at( std::size_t index ) const
  {
    if( index >= mNumberOfSignals ) 
    {
      throw std::out_of_range( "Index exceeds the number of signals" );
    }
    return operator[]( index );
  }

private:
  const std::size_t mNumberOfSignals;
  const std::size_t mSignalLength;
  const std::size_t mSignalStride;
  efl::AlignedArray<SampleType> mStorage;

};

} // namespace rrl
} // namespace visr

#endif // VISR_RRL_COMMUNICATION_AREA_HPP_INCLUDED
