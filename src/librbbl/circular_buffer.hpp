/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_CIRCULAR_BUFFER_HPP_INCLUDED
#define VISR_LIBRBBL_CIRCULAR_BUFFER_HPP_INCLUDED

#include <libefl/basic_matrix.hpp>

#include <cassert>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace rbbl
{

/**
 * A generic data type to emulate contiguous memory buffers.
 * It enables a linear, wraparound-free access to the whole allocated size of the buffer.
 * It is implemented as a multichannel buffer with an arbitrary number of channels and a 
 * common write position for all channels. Read access is relative to the write position and can be channel-specific.
 * Note that the write index is at the position of the next write operation, i.e., one in front of the most recently 
 * written input.
 * This class is optimized for multiple read accesses (i.e., more frequent random reads than write operations.)
 * The basic C++ implementation duplicates the written data two times, but platform-specific implementations might be more sophisticated, e.g., 
 * combining shared memory segments to form a contiguous memory area.
 * @tparam DataType The floating-point element type for the contained samples.
 */
template< typename DataType >
class CircularBuffer
{
public:
  /**
   * The index type used to address the read samples relative to the write index.
   * Positive values denote past samples. Since this type is unsigned, passing a negative 
   * value to this parameter results in a hugely misplaced read operation.
   */
  using DelayIndexType = std::size_t;

  /**
   * Constructor.
   * @param numberOfChannels The number of simultaneous channels contained in the circular buffer.
   * @param length The requested size of the ringbuffer. The actual length might be larger, but this does not affect
   * the functionality of the ringbuffer (if not accessed outside the requested size).
   * @param alignment The alignment in multiples of the element data type (must be an integer power of two). Determines the address of the underlying memory 
   * buffer and possibly also the total length of the ringbuffer, i.e., the buffer is padded to the next multiple of the alignment. 
   */
  explicit CircularBuffer( std::size_t numberOfChannels, std::size_t length, std::size_t alignment = 0 );

  /**
   * Destructor.
   */
  ~CircularBuffer();

  /**
   * Return the number of simultaneous data channels.
   */
  std::size_t numberOfChannels() const { return mNumberOfChannels; }

  /**
   * Return the (requested) length of the ringbuffer.
   */
  std::size_t length() const { return mLength; }

  /**
   * Return the address difference (in multiples of the element size) 
   * between the same position for two adjacent channels.
   */
  std::size_t stride() const { return mStride; }

  /**
   * Write a given number of samples for all contained channels into the circular buffer and 
   * advance the write pointer afterwards.
   * @param writeData The data to be written. The vector must contain at least \p numberOfChannels pointers,
   * and each pointers must point to an array of at least \p numberOfSamples samples.
   * @param numberOfChannels The number of signal channels, must match the number of channels of the matrix.
   * @param numberOfSamples The number of samples per channel to be written.
   * @param alignmentElements A alignment guarantee (measure in samples) for the arrays pointed  to by writeData.
   * @throw std::invalid_argument If the \p numberOfChannels argument does not match the object's number of channels,
   * or if \p numberOfSamples exceeds the requested length of the circular buffer.
   */
  void write( DataType const * const * writeData,
              std::size_t numberOfChannels,
              std::size_t numberOfSamples,
              std::size_t alignmentElements = 0 );

  /**
   * Write samples from a matrix object into the circular buffer and advance the write index afterwards.
   * @param writeData A matrix containing the samples to be written.
   * @throw std::invalid_argument If \p writeData.numberOfChannels() does not match the object's number of channels,
   * or if \p writeData.numberOfColumns() exceeds the requested length of the circular buffer.
   * @note The alignment of the input matrix is determined from the alignElements() method of writeData.
   */
  void write( efl::BasicMatrix<DataType> const & writeData );

  /**
   * Get the address of the current write position for the first channel.
   * The positions of the other channels can be determinde from this value using \p stride().
   * This method can be used to write directly into the matrix.
   * @note To take effect, writing the data into the buffer must be concluded by a \p commitWriteOperation() call.
   * @note Read accesses between writing new data and calling \p commitWriteOperation() might lead to incorrect values, 
   * because data within the allocated length of the ringbuffeer might be overwritten before advancing the write pointer.
   */
  DataType * getFirstWritePointer();

  /**
  * Get the addresses of the current write position for all channels.
  * This method can be used to write directly into the matrix.
  * @param[out] writePointers The write positions. The argument must provide room for \p numberOfChannels() pointers.
  * @note To take effect, writing the data into the buffer must be concluded by a \p commitWriteOperation call.
  */
  void getWritePointers( DataType * * writePointers );

  /**
   * Finish a write operation by advancing the write index.
   * Depending on the underlying operation, this call might also trigger other operations,
   * e.g., copying/duplication of sample data.
   * @param numberOfElementsWritten The advance of the write index (given as number of elements)
   */
  void commitWrite( std::size_t numberOfElementsWritten );

  /**
   * Retrieve read pointers relative to the current write position.
   * @param delay Common delay for all channels. As the write index is one position after the last written value, a delay value of 1
   * corresponds to the most recently written value.
   * @param[out] readPointers A pointer array to hold the read positions for all channels. Must provide space for \p numberOfChannels() elements.
   * @throw std::invalid_argument If \p delay exceeds the length of the circular buffer.
   */
  void getReadPointers( DelayIndexType delay, DataType const * * readPointers ) const;

  /**
  * Retrieve read pointers relative to the current write position.
  * @param delay An array of delay values, one for each channel. Must contain \p numberOfChannels() elements.
  * As the write index is one position after the last written value, a delay value of 1
  * corresponds to the most recently written value.
  * @param[out] readPointers A pointer array to hold the read positions for all channels. Must provide space for \p numberOfChannels() elements.
  * @throw std::invalid_argument If any element of \p delay exceeds the length of the circular buffer.
  */
  void getReadPointers( DelayIndexType const * const delay, DataType const * * readPointers ) const;

  /**
   * Retrieve a read pointer for a certain channel.
   * @param channelIndex The channel for which the pointer is requested.
   * @param delay The delay (relative to the write position)
   * @note This method is inline and does not check for the correct range of the arguments (except in debug builds).
   */
  DataType const * getReadPointer( std::size_t channelIndex,
                                   DelayIndexType delay ) const
  { 
    assert( channelIndex < numberOfChannels() );
    assert( delay <= length() );
    return (mBasePointer + channelIndex * mStride + mAllocatedLength + mWriteHeadIndex) - delay;
  }

private:
  /**
   * Private implementation class, encapsulates the internal data structures and implementation for a certain platform.
   */
  class Impl;
  /**
   * Pointer to the implementation object (pimpl idiom).
   */
  std::unique_ptr<Impl> mImpl;

  /**
   * The requested length of the circular buffer. All accesses ara checked against this value.
   */
  std::size_t const mLength;

  /**
   * The actual length of the circular buffer, equal or larger than the requested length.
   */
  std::size_t const mAllocatedLength;

  /**
   * The difference (in number of elements) beween the elements at the same position in two adjacent channels.
   */
  std::size_t const mStride;

  /**
   * The number of simultaneous signal channels contained in the object.
   */
  std::size_t const mNumberOfChannels;

  /**
   * The pointer to the first (i.e., zeroth) element of the first (zeroth) channel.
   * Retrieved from the \p Impl object for efficiency reasons.
   */
  DataType * const mBasePointer;

  /**
   * That is, the sample corresponding to delay=0 is 'to the left' of the write index.
   */
  std::size_t mWriteHeadIndex;
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_CIRCULAR_BUFFER_HPP_INCLUDED
