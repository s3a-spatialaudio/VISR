/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_FFTW_WRAPPER_HPP_INCLUDED
#define VISR_LIBRBBL_FFTW_WRAPPER_HPP_INCLUDED

#include <libefl/basic_matrix.hpp>

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace rbbl
{

/**
 * @tparam DataType The floating-point element type for the contained samples.
 */
template< typename DataType >
class CircularBuffer
{
public:
  using DelayIndexType = std::size_t;

  explicit CircularBuffer( std::size_t numberOfChannels, std::size_t length, std::size_t alignment = 0 );

  ~CircularBuffer();

  std::size_t numberOfChannels() const { return mNumberOfChannels; }

  std::size_t length() const { return mLength; }

  std::size_t stride() const { return mStride; }

  void write( DataType const * const * writeData,
              std::size_t numberOfChannels,
              std::size_t numberOfSamples,
              std::size_t alignmentElements = 0 );

  void write( efl::BasicMatrix<DataType> const & writeData,
              std::size_t alignmentElements = 0 );

  DataType * getFirstWritePointer();

  void getWritePointers( DataType * * writePointers );

  void commitWrite( std::size_t numberOfElementsWritten );

  void getReadPointers( DelayIndexType delay, DataType const * * readPointers ) const;

  void getReadPointers( DelayIndexType const * const delay, DataType const * * readPointers ) const;

  DataType const * getReadPointer( std::size_t channelIndex,
                                   DelayIndexType delay ) const
  { 
    return (mBasePointer + channelIndex * mStride + mAllocatedLength + mWriteHeadIndex - 1) - delay;
  }

private:
  /**
   *
   */
  class Impl;
  /**
   * Pointer to the implementation object (pimpl idiom).
   */
  std::unique_ptr<Impl> mImpl;

  std::size_t const mLength;

  std::size_t const mAllocatedLength;

  std::size_t const mStride;

  std::size_t const mNumberOfChannels;

  DataType * const mBasePointer;

  /**
   * That is, the sample corresponding to delay=0 is 'to the left' of the write index.
   */
  std::size_t mWriteHeadIndex;
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_FFTW_WRAPPER_HPP_INCLUDED
