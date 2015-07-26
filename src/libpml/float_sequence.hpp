/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_FLOAT_SEQUENCE_HPP_INCLUDED
#define VISR_PML_FLOAT_SEQUENCE_HPP_INCLUDED

#include <algorithm>
#include <ciso646>
#include <initializer_list>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace pml
{

/**
 * Sequence of floating-point sequence that can be parsed from strings.
 * @tparam The (floating-point) element type. Instantiations existfor float and double.
 */
template<typename ElementType>
class FloatSequence
{
public:
  using ContainerType = std::vector<ElementType>;
  
  /**
   * Default constructor, creates empty list.
   */
  FloatSequence();

  /**
   * Construct sequence with \p num repetitions of \p val.
   */
  explicit FloatSequence( ElementType val, std::size_t num = 1);
  
  FloatSequence( std::initializer_list<ElementType> const & val );

  explicit FloatSequence( std::string const & val );

  std::size_t size() const 
  {
    return mIndices.size();  
  }

  ElementType * values()
  {
    return &mIndices[0];
  }

  ElementType const * values() const
  {
    return &mIndices[0];
  }
  
  typename ContainerType::const_iterator begin() const 
  {
    return mIndices.begin();
  }
  
  typename ContainerType::const_iterator end() const
  {
    return mIndices.end();
  }
  
  typename ContainerType::iterator begin() 
  {
    return mIndices.begin();
  }
  
  typename ContainerType::iterator end() 
  {
    return mIndices.end();
  }
  
  /**
   * Element access without bounds checking
   */
  ElementType & operator[]( std::size_t idx )
  {
    return mIndices[idx];
  }

  /**
   * Element access without bounds checking, constant version
   */
  ElementType const & operator[]( std::size_t idx ) const 
  {
    return mIndices[idx];
  }

  /**
   * Element access with bounds checking
   */
  ElementType & at( std::size_t idx ) 
  {
    return mIndices.at( idx );
  }

  /**
   * Element access with bounds checking, constant version
   */
  ElementType const & at( std::size_t idx ) const 
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

} // namespace pml
} // namespace visr


#endif // VISR_PML_FLOAT_SEQUENCE_HPP_INCLUDED
