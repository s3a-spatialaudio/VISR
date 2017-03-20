/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_CHANNEL_LIST_HPP_INCLUDED
#define VISR_CHANNEL_LIST_HPP_INCLUDED

#include "export_symbols.hpp"

#include <iterator>
#include <initializer_list>
#include <vector>

namespace visr
{

// Forward declaration
class ChannelRange;

class VISR_CORE_LIBRARY_SYMBOL ChannelList
{
public:
  using IndexType = std::size_t;

  ChannelList();

  /**
  * Construct a channel list from a single channel range.
  * This constructor ius intended to support
  */
  ChannelList( ChannelRange const & range );

  ChannelList( std::initializer_list<IndexType> const & initList );

  template< class Container>
  ChannelList( Container const & container )
    : ChannelList( std::begin(container), std::end(container) )
  {
  }

  template< class Iterator>
  ChannelList( Iterator begin, Iterator end )
  {
    for( ; begin != end; ++begin )
    {
      appendIndex( *begin );
    }
  }


  ChannelList( std::initializer_list<ChannelRange> const & initList );

  std::size_t size() const;

  IndexType operator[]( std::size_t idx ) const;

  IndexType& operator[]( std::size_t idx );

  IndexType at( std::size_t idx ) const;

  IndexType& at( std::size_t idx );

  using ListType = std::vector<IndexType>;
  using const_iterator = ListType::const_iterator;

  const_iterator begin() const { return mChannels.begin(); }
  const_iterator end() const { return mChannels.end(); }

private:


  ListType mChannels;

  void appendIndex( IndexType index );

  void appendRange( ChannelRange const & range );
};

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
  */
  ChannelRange( IndexType val );

  /**
  * Construct a range from start, end, and optional step values
  * @throw std::invalid_argument the range specification is invalid (i.e., if step is zero or the increments goes in the different direction of the end value.)
  * @note this constructor is not explicit to enable construction of ChannelList objects from initializer lists of ranges.
  */
  ChannelRange( IndexType start, IndexType end, StepType step = 1 );

  /**
   * Set a range using start, end, and (optional) step notation
   * @throw std::invalid_argument the range specification is invalid.
   */
  void set( IndexType start, IndexType end, StepType stride = 1 );

  /**
  * Static method to check whether a range specification is valid.
  */
  static bool isValid( IndexType start, IndexType size, StepType stride );

  /**
   * Query whether the range is valid, i.e., whether it contains a finite (potentially zero) number of elements.
   */
  bool isValid() const;

  /**
  */
  IndexType size() const;

  IndexType start() const { return mStart; }

  IndexType end() const { return mEnd; }

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

} // namespace visr

#endif // #ifndef VISR_CHANNEL_LIST_HPP_INCLUDED
