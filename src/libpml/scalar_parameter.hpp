/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_SCALAR_PARAMETER_HPP_INCLUDED
#define VISR_PML_SCALAR_PARAMETER_HPP_INCLUDED

#include "empty_parameter_config.hpp"
#include "export_symbols.hpp"

#include <libril/parameter_type.hpp>
#include <libril/typed_parameter_base.hpp>

#include <complex>

namespace visr
{
namespace pml
{

namespace // unnamed
{
/**
 * Type trait to assign a unique type id to each concrete ScalarParameter template instantiation.
 */
template<typename ElementType> struct ScalarParameterType {};

template<> struct ScalarParameterType<bool>
{
  static constexpr const ParameterType ptype() { return detail::compileTimeHashFNV1( "Boolean" ); }
};

template<> struct ScalarParameterType<int>
{
  static constexpr const ParameterType ptype() { return detail::compileTimeHashFNV1( "Integer" ); }
};

template<> struct ScalarParameterType<unsigned int>
{
  static constexpr const ParameterType ptype() { return detail::compileTimeHashFNV1( "UnsignedInteger" ); }
};

template<> struct ScalarParameterType<float>
{ static constexpr const ParameterType ptype() { return detail::compileTimeHashFNV1( "Float" ); } };

template<> struct ScalarParameterType<double>
{ static constexpr const ParameterType ptype() { return detail::compileTimeHashFNV1( "Double" ); } };

template<> struct ScalarParameterType<std::complex<float> >
{ static constexpr const ParameterType ptype() { return detail::compileTimeHashFNV1( "ComplexFloat" ); } };

template<> struct ScalarParameterType<std::complex<double> >
{ static constexpr const ParameterType ptype() { return detail::compileTimeHashFNV1( "ComplexDouble" ); } };
} // unnamed namespace

/**
 * A type for passing vectors between processing components.
 * The template class is explicitly instantiated for the element types float and double.
 * @tparam ElementType The data type of the elements of the matrix.
 */
template<typename ElementType >
class VISR_PML_LIBRARY_SYMBOL ScalarParameter: public TypedParameterBase<ScalarParameter<ElementType>, EmptyParameterConfig, ScalarParameterType<ElementType>::ptype() >
{
public:
  /**
   * Default constructor.
   * In case of an elementary type, this initialises the contained scalar to an arbitrary value.
   */
  ScalarParameter();

  ScalarParameter( ScalarParameter<ElementType> const & rhs );

  explicit ScalarParameter(ParameterConfigBase const & config);

  explicit ScalarParameter(EmptyParameterConfig const & config);

  explicit ScalarParameter( ElementType val );

  virtual ~ScalarParameter() override;

  ElementType &  operator=(ElementType val );

  ElementType value() const;
private:
  ElementType mValue;
};

} // namespace pml
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::pml::ScalarParameter<bool>, visr::pml::ScalarParameter<bool>::staticType(), visr::pml::EmptyParameterConfig )
DEFINE_PARAMETER_TYPE( visr::pml::ScalarParameter<unsigned int>, visr::pml::ScalarParameter<unsigned int>::staticType(), visr::pml::EmptyParameterConfig )
DEFINE_PARAMETER_TYPE( visr::pml::ScalarParameter<int>, visr::pml::ScalarParameter<int>::staticType(), visr::pml::EmptyParameterConfig )
DEFINE_PARAMETER_TYPE( visr::pml::ScalarParameter<float>, visr::pml::ScalarParameter<float>::staticType(), visr::pml::EmptyParameterConfig )
DEFINE_PARAMETER_TYPE( visr::pml::ScalarParameter<double>, visr::pml::ScalarParameter<double>::staticType(), visr::pml::EmptyParameterConfig )
DEFINE_PARAMETER_TYPE( visr::pml::ScalarParameter< std::complex<float> >, visr::pml::ScalarParameter< std::complex<float> >::staticType(), visr::pml::EmptyParameterConfig )
DEFINE_PARAMETER_TYPE( visr::pml::ScalarParameter< std::complex<double> >, visr::pml::ScalarParameter< std::complex<double> >::staticType(), visr::pml::EmptyParameterConfig )

#endif // VISR_PML_SCALAR_PARAMETER_HPP_INCLUDED
