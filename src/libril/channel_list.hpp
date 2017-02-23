/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_CHANNEL_LIST_HPP_INCLUDED
#define VISR_LIBRIL_CHANNEL_LIST_HPP_INCLUDED

#include <iterator>
#include <initializer_list>
#include <vector>

namespace visr
{
namespace ril
{
// Forward declaration
class ChannelRange;

class ChannelList
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

class ChannelRange
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
  */
  ChannelRange( IndexType val );

  explicit ChannelRange( IndexType start, IndexType end, StepType step = 1 );

  /**
  * @throw std::invalid_argument the range specification is invalid.
  */
  void set( IndexType start, IndexType size = 1, StepType stride = 1 );

  /**
  * Static method to check whether a range specification is valid.
  */
  static bool isValid( IndexType start, IndexType size, StepType stride );

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

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_CHANNEL_LIST_HPP_INCLUDED
