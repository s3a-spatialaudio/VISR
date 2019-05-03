/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_BASE_HPP_INCLUDED
#define VISR_PARAMETER_BASE_HPP_INCLUDED

#include "export_symbols.hpp"
#include "parameter_type.hpp"

#include <memory>

namespace visr
{

/**
 * Base class for parameter data types.
 * Abstract base class.
 */
class VISR_CORE_LIBRARY_SYMBOL ParameterBase
{
public:

  ParameterBase();

  /**
   * Copy constructor.
   * This uses the default implementation.
   * This function needs to be implemented explicitly because the class is exported.
   */
  ParameterBase( const ParameterBase & );

  ParameterBase& operator=( const ParameterBase & );

  /**
   *
   */
  virtual ~ParameterBase();

  /**
   * Return the dynamic type of the parameter object.
   */
  virtual ParameterType type() = 0;

  /**
   * Virtual copy constructor interface, returns a pointer
   * to a copy of the derived object.
   */
  virtual std::unique_ptr<ParameterBase> clone() const = 0;

  /**
   * Virtual assingment operator.
   * Pure virtual member function, must be implemented in derived classes.
   * @param rhs The object to be copied.
   * @throw std::invalid_argument if the type of \p rhs does not match the type of this object.
   */
  virtual void assign( ParameterBase const & rhs ) = 0;
};

} // namespace visr

#endif // #ifndef VISR_PARAMETER_BASE_HPP_INCLUDED
