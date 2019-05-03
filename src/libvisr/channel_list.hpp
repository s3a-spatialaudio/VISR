/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_CHANNEL_LIST_HPP_INCLUDED
#define VISR_CHANNEL_LIST_HPP_INCLUDED

#include "export_symbols.hpp"

#include <iterator>
#include <initializer_list>
#include <ostream>
#include <vector>

namespace visr
{

// Forward declaration
class ChannelRange;

/**
 * Class representing a list of channel indices.
 * It is used to specify arbitrary connections and channel routings between audio ports.
 * @see CompositeComponent::audioConnection
 */
class VISR_CORE_LIBRARY_SYMBOL ChannelList
{
public:
  using IndexType = std::size_t;

  /**
   * Default constructor, creates an empty channel list.
   */
  ChannelList();

  /**
  * Construct a channel list from a single channel range, i.e., from a start, stop, stride description.
  * This constructor is supposed to support type conversion, for instance to enable the use of a ChannelRange object in a
  * CompositeComponent::audioConnection() call.
  * Therefore it is not marked as explicit.
  * @param range A ChannelRange object defining an equidistantly strided range of channels.
  */
  ChannelList( ChannelRange const & range );

  /**
   * Construct a channel list from an initialiser list.
   * This constructor is suppoed to be used for type conversion, for instance to enable an initialiser list as argument in a
   * CompositeComponent::audioConnection call.
   * Therefore it is not marked as explicit.
   * @param initList Initaliser list of channels indices, typically a comma,separated sequence enclosed in curly braces.
   */
  ChannelList( std::initializer_list<IndexType> const & initList );

  /**
   * Construct a channel list from a initialiser list of ChannelRange objects.
   * This constructor is to support implicit type conversion, for instance to use an initialiser list directly in 
   * a CompositeComponent::audioConnection call.
   * @param initList Initialiser list of ChannelRange objects, typically enclosed in curly braces.
   */
  ChannelList( std::initializer_list<ChannelRange> const & initList );

  /**
  * Construct a channel list from a container.
  * @note This template member function is not an export of the shared library because it is
  * instantiated at the caller site.
  * @note With C++11, the implementation might use std::cbegin() and std::cend() instead.
  * @tparam Container Array type for which std::begin() and std::end() are specialized and whose
  * value type is convertible to the index type.
  * @param container Container containing indices (or data convertible to indices)
  */
  template< class Container>
  ChannelList( Container const & container )
    : ChannelList( std::begin(container), std::end(container) )
  {
  }

  /**
   * Construct a channel list from a sequence.
   * @tparam Iterator Iterator type matching the forward iterator concept and whose 
   * referenced type is convertible to the index type.
   * @param begin Begin iterator
   * @param end Past-the-end iterator
   */
  template< class Iterator>
  ChannelList( Iterator begin, Iterator end )
  {
    for( ; begin != end; ++begin )
    {
      appendIndex( *begin );
    }
  }

  /**
   * Destructor.
   */
  ~ChannelList();

  /**
   * Return the number of elements in the channel list.
   */
  std::size_t size() const;

  /**
   * Unchecked index access to the channel indices, const version.
   * Does not check whether idx is within the permissible range.
   * @param idx Zero-offset position index within the channel list.
   */
  IndexType operator[]( std::size_t idx ) const;

  /**
  * Unchecked index access to the channel indices.
  * Does not check whether idx is within the permissible range.
  * @param idx Zero-offset position index within the channel list.
  */
  IndexType& operator[]( std::size_t idx );

  /**
   * Bounds-checking index access to the channel indices, const version.
   * @throw std::out_of_range If \p idx exceeds the number of indices in the list.
   * @param idx Zero-offset position index within the channel list.
   */
  IndexType at( std::size_t idx ) const;

  /**
   * Bounds-checking index access to the channel indices.
   * @throw std::out_of_range If \p idx exceeds the number of indices in the list.
   * @param idx Zero-offset position index within the channel list.
   */
  IndexType& at( std::size_t idx );

  using ListType = std::vector<IndexType>;
  using const_iterator = ListType::const_iterator;

  /**
   * Return a const iterator to the beginning of the index list.
   * @todo Check whether we should provide a cbegin() version as well.
   * @todo Check whether we should provide a non-const version too.
   */
  const_iterator begin() const { return mChannels.begin(); }

  /**
  * Return a const iterator to the end of the index list.
  * @todo Check whether we should provide a cend() version as well.
  * @todo Check whether we should provide a non-const version too.
  */
  const_iterator end() const { return mChannels.end(); }
private:

  /**
   * The container for the channels indices.
   */
  ListType mChannels;

  /**
   * Internal function for appending indices to the end of the index list.
   * Its basic purpose is to move the actual implementation of the templated constructors accepting containers and iterator 
   * ranges outside the header file.
   */
  void appendIndex( IndexType index );

  /**
   * Interanl function to append a channel range to the end of the channel list.
   */
  void appendRange( ChannelRange const & range );
};

/**
 * Stream output operator for ChannelList objects.
 * Creates a list of channel indices. Sequences of three or more equidistant consecutive indices are contracted into
 * stride expressions start:step:end or start:end in case of a unit stride.
 */
VISR_CORE_LIBRARY_SYMBOL std::ostream & operator<<( std::ostream & str, ChannelList const channels );

/**
 * Class representing a sequence of equidistant channel indices caharcterised by a start, an end, and step (or stride) value.
 * The end value is not included in the range following the 'one-past-end' semantics used in the STL or Python.
 * Strides can be negative but must be non-zero.
 */
class VISR_CORE_LIBRARY_SYMBOL ChannelRange
{
public:
  using IndexType = ChannelList::IndexType;
  using StepType = std::ptrdiff_t;

  /**
  * Default constructor, construct an empty range.
  */
  ChannelRange();

  /**
  * Construct a single-element range.
  * @note this constructor is not explicit to enable construction of ChannelList objects from initializer lists.
  * @param val The start valu (and sole value of the resulting range)
  */
  ChannelRange( IndexType val );

  /**
  * Construct a range from start, end, and optional step values
  * @param start The start value of the sequence.
  * @param end The end value of the sequence. this value is not included in the index sequence.
  * @param step The increment between successive channel indices, must be non-zero.
  * @throw std::invalid_argument the range specification is invalid (i.e., if step is zero or the increments goes in the different direction of the end value.)
  * @note this constructor is not explicit to enable construction of ChannelList objects from initializer lists of ranges.
  */
  ChannelRange( IndexType start, IndexType end, StepType step = 1 );

  /**
   * Set a range using start, end, and (optional) step notation
   * @param start The start value of the sequence.
   * @param end The end value of the sequence. this value is not included in the index sequence.
   * @param stride The increment between successive channel indices, must be non-zero.
   * @throw std::invalid_argument the range specification is invalid (i.e., if step is zero or the increments goes in the different direction of the end value.)
   * @see isValid
   */
  void set( IndexType start, IndexType end, StepType stride = 1 );

  /**
  * Static method to check whether a range specification is valid.
  */
  static bool isValid( IndexType start, IndexType size, StepType stride );

  /**
   * Query whether the range is valid, i.e., whether it contains a finite (potentially zero) number of elements.
   * That is, ranges are invalid if the stride is zero or the end index cannot be reached from the start index with the given stride value.
   */
  bool isValid() const;

  /**
   * Return the number of channel indices in the range.
   */
  IndexType size() const;

  /**
   * Return the start value of the range.
   */
  IndexType start() const { return mStart; }

  /**
   * Return the end value of the sequence.
   * @note This is different from the last element of the sequence, because the class follows the 'one-past-the-end' semantics used in the STL and Python.
   */
  IndexType end() const { return mEnd; }

  /**
   * Return the step size (stride) between consecutive elements.
   */
  StepType step() const { return mStep; }

  /**
  * Return the channel index at the \p idx-th position.
  * @throw std::out_of_range if \p idx exceeds the size of the slice
  */
  IndexType at( IndexType idx ) const;

  /**
  * Return the channel index at the \p idx-th position.
  * @note No checking is performed whrther \p idx exceeds the size of the slice.
  */
  IndexType operator[]( IndexType idx ) const;

  /**
  * Return the channel index at the \p idx-th position.
  * @throw std::out_of_range if \p idx exceeds the size of the slice
  */
  IndexType at( IndexType idx );

  /**
  * Return the channel index at the \p idx-th position.
  * @note No checking is performed whrther \p idx exceeds the size of the slice.
  */
  IndexType operator[]( IndexType idx );

private:
  IndexType mStart;
  IndexType mEnd;
  StepType mStep;
};

/**
 * Stream output operator for ChannelRange objects.
 */
VISR_CORE_LIBRARY_SYMBOL std::ostream & operator<<( std::ostream & str, ChannelRange const range );

} // namespace visr

#endif // #ifndef VISR_CHANNEL_LIST_HPP_INCLUDED
