/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_RBBL_INDEX_SEQUENCE_HPP_INCLUDED
#define VISR_RBBL_INDEX_SEQUENCE_HPP_INCLUDED

#include <algorithm>
#include <ciso646>
#include <initializer_list>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace rbbl
{

/**
 *
 */
class IndexSequence
{
public:
  using IndexType = std::size_t;

  using ContainerType = std::vector<IndexType>;
  
  /**
   * Default constructor, creates empty list.
   */
  IndexSequence();
  
  IndexSequence( std::initializer_list<IndexType> const & val );

  IndexSequence( std::string const & val );

  std::size_t size() const 
  {
    return mIndices.size();  
  }

  IndexType * values()
  {
    return &mIndices[0];
  }

  IndexType const * values() const
  {
    return &mIndices[0];
  }
  
  ContainerType::const_iterator begin() const 
  {
    return mIndices.begin();
  }
  
  ContainerType::const_iterator end() const
  {
    return mIndices.end();
  }
  
  ContainerType::iterator begin() 
  {
    return mIndices.begin();
  }
  
  ContainerType::iterator end() 
  {
    return mIndices.end();
  }
  
  /**
   * Element access without bounds checking
   */
  IndexType & operator[]( std::size_t idx )
  {
    return mIndices[idx];
  }

  /**
   * Element access without bounds checking, constant version
   */
  IndexType const & operator[]( std::size_t idx ) const 
  {
    return mIndices[idx];
  }

  /**
   * Element access with bounds checking
   */
  IndexType & at( std::size_t idx ) 
  {
    return mIndices.at( idx );
  }

  /**
   * Element access with bounds checking, constant version
   */
  IndexType const & at( std::size_t idx ) const 
  {
    return mIndices.at( idx );
  }
  
  /**
   * Reset the list to an empty state.
   */
  void clear();


private:
  ContainerType mIndices;
};

} // namespace rbbl
} // namespace visr


#endif // VISR_RBBL_INDEX_SEQUENCE_HPP_INCLUDED
