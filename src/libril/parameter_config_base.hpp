/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_CONFIG_BASE_HPP_INCLUDED
#define VISR_PARAMETER_CONFIG_BASE_HPP_INCLUDED

#include "parameter_base.hpp"

#include "export_symbols.hpp"

#include <memory>

namespace visr
{

/**
 * Base class for parameter configuration objects.
 * A parameter configuration object adds information about the transmitted parameter objects to a parameter port or communication protocol.
 * This information contains additional information about the parameter (e.g., the dimension of the matrix), but not the type itself. Each parameter class is associated 
 * with a specific parameter configuration type, but the same parameter configuration class can be potentially used by multiple parameter classes.
 * Parameter configuration objects are used by the runtime system to check compatibility of parameter connections, and to construct parameter objects.
 */
class ParameterConfigBase
{
protected:
  /**
   * Default constructor.
   * This constructor is protected because only derived classes can be instantiated.
   */
  VISR_CORE_LIBRARY_SYMBOL ParameterConfigBase();

  /**
   * Copy constructor.
   * This constructor is protected because only derived classes can be instantiated.
   * @note It is defined explicitly in order to have the symbol placed in the library
   */
  VISR_CORE_LIBRARY_SYMBOL ParameterConfigBase( ParameterConfigBase const & );

public:
  /**
   * Destructor (virtual).
   * Parameter configuration objects are instantiated and managed polymorphically, therefore the destructor has to be virtual
   */
  VISR_CORE_LIBRARY_SYMBOL virtual ~ParameterConfigBase();

  /**
   * Comparison function between parameter configurations.
   * Must only be called between objects of equal dynamic type.
   * Pure virtual function interface, must be implemented by derived parameter config types.
   * @param rhs The parameter configuration object to compare with.
   * @return True if the parameter objects are compatible, false otherwise
   * @throw std::invalid_argument it the \p this object and \p rhs have nonmatching dynamic types.
   */
  virtual bool compare( ParameterConfigBase const & rhs) const = 0;

  /**
   * Clone (virtual copy construction) function.
   * Pure virtual function, must be defined in derived types.
   * @return A shared pointer to an object of the derived type.
   */
  virtual std::unique_ptr<ParameterConfigBase> clone() const = 0;
};

} // namespace visr

#endif // #ifndef VISR_TYPED_PARAMETER_BASE_HPP_INCLUDED
