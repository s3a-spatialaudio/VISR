/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_RBBL_FLOAT_SEQUENCE_HPP_INCLUDED
#define VISR_RBBL_FLOAT_SEQUENCE_HPP_INCLUDED

#include <algorithm>
#include <ciso646>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <vector>

namespace visr
{
namespace rbbl
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

  /**
   * Construct a float sequence out of a C-style array with a given number of elements.
   * @param val Pointer to the data array.
   * @param numValues Number of data elements.
   */
  explicit FloatSequence( ElementType const * const val, std::size_t numValues );

  /**
   * Construct a float sequence from a C++ initializer list.
   * @param val initializer list (literal list of floating point values as { 0.0, 0.3, 0.4 }
   */
  explicit FloatSequence( std::initializer_list<ElementType> const & val );

  /**
   * Create an object from an a string representation.
   * @param val A string containig a comma-separated sequence of float values or Matlab-style ranges "start:stride:end"
   */
  explicit FloatSequence( std::string const & val );

  std::size_t size() const 
  {
    return mValues.size();
  }

  ElementType * values()
  {
    return &mValues[0];
  }

  ElementType const * values() const
  {
    return &mValues[0];
  }
  
  typename ContainerType::const_iterator begin() const 
  {
    return mValues.begin();
  }
  
  typename ContainerType::const_iterator end() const
  {
    return mValues.end();
  }
  
  typename ContainerType::iterator begin() 
  {
    return mValues.begin();
  }
  
  typename ContainerType::iterator end() 
  {
    return mValues.end();
  }
  
  /**
   * Element access without bounds checking
   */
  ElementType & operator[]( std::size_t idx )
  {
    return mValues[idx];
  }

  /**
   * Element access without bounds checking, constant version
   */
  ElementType const & operator[]( std::size_t idx ) const 
  {
    return mValues[idx];
  }

  /**
   * Element access with bounds checking
   */
  ElementType & at( std::size_t idx ) 
  {
    return mValues.at( idx );
  }

  /**
   * Element access with bounds checking, constant version
   */
  ElementType const & at( std::size_t idx ) const 
  {
    return mValues.at( idx );
  }
  
  /**
   * Reset the list to an empty state.
   */
  void clear();

  /**
   * Transform the contents ito a textual representation.
   * @param separator The separator put in between adjacent values.
   * @return String representation of the floating-point sequence
   */
  std::string toString( std::string const & separator=std::string(", ") ) const;
private:
  ContainerType mValues;
};

} // namespace rbbl
} // namespace visr


#endif // VISR_RBBL_FLOAT_SEQUENCE_HPP_INCLUDED
