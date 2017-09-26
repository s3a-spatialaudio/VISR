/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libvisr/parameter_base.hpp>
#include <libvisr/parameter_type.hpp>


#include <pybind11/pybind11.h>

#include <ciso646>
#include <iostream> // For debugging purposes only.

namespace visr
{
namespace python
{
namespace visr
{

/**
 * Wrapper class to get access to the full functionality
 * Apparently nor required anymore (and is troublesome when deducing the argument
 * type).
 */
class ParameterBaseWrapper: public ParameterBase
{
public:
  /**
   * Use base class constructors
   */
  using ParameterBase::ParameterBase;

};

void exportParameterBase( pybind11::module& m )
{
  pybind11::class_<ParameterBase/*, ParameterBaseWrapper*/>( m, "ParameterBase" )
    .def( "type", &ParameterBase::type )
    ;
}

} // namepace visr
} // namespace python
} // namespace visr

