/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_INTERPOLATION_PARAMETER_HPP_INCLUDED
#define VISR_PML_INTERPOLATION_PARAMETER_HPP_INCLUDED

#include "export_symbols.hpp"

#include <librbbl/interpolation_parameter.hpp> 

#include <libvisr/parameter_type.hpp>
#include <libvisr/parameter_config_base.hpp>
#include <libvisr/typed_parameter_base.hpp>

#include <limits>
#include <vector>

namespace visr
{
namespace pml
{

namespace // unnamed
{
} // unnamed namespace


  /**
  * A type for encapsulating the configuration of a vector parameter.
  */
class VISR_PML_LIBRARY_SYMBOL InterpolationParameterConfig: public ParameterConfigBase
{
public:
  explicit InterpolationParameterConfig( std::size_t numberOfInterpolants );

  virtual ~InterpolationParameterConfig();

  std::unique_ptr< ParameterConfigBase > clone() const override;

  bool compare( ParameterConfigBase const & rhs ) const override;

  bool compare( InterpolationParameterConfig const & rhs ) const;

  std::size_t numberOfInterpolants() const { return mNumberOfInterpolants; }
private:
  std::size_t mNumberOfInterpolants;
};

/**
 * A type for passing vectors between processing components.
 * The template class is explicitly instantiated for the element types float and double.
 * @tparam ElementType The data type of the elements of the matrix.
 */
class VISR_PML_LIBRARY_SYMBOL InterpolationParameter:
  public TypedParameterBase<InterpolationParameter, InterpolationParameterConfig, detail::compileTimeHashFNV1("InterpolationParameter") >,
  public rbbl::InterpolationParameter
{
public:
  /**
   * Make all constructors of rbbl::InterpolationParameter accessible
   */
  using rbbl::InterpolationParameter::InterpolationParameter;

  explicit InterpolationParameter(ParameterConfigBase const & config);

  explicit InterpolationParameter(InterpolationParameterConfig const & config);

};

} // namespace pml
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::pml::InterpolationParameter, visr::pml::InterpolationParameter::staticType(), visr::pml::InterpolationParameterConfig )

#endif // VISR_PML_INTERPOLATION_PARAMETER_HPP_INCLUDED
