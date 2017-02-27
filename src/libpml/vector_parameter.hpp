/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_VECTOR_PARAMETER_HPP_INCLUDED
#define VISR_PML_VECTOR_PARAMETER_HPP_INCLUDED

#include "vector_parameter_config.hpp"

#include <libefl/basic_vector.hpp>

#include <libril/parameter_type.hpp>
#include <libril/typed_parameter_base.hpp>

#include <initializer_list>
#include <istream>

namespace visr
{
namespace pml
{

/**
 * A type for passing matrixes between processing components.
 * The template class is explicitly instantiated for the element types float and double.
 * @tparam ElementType The data type of the elements of the matrix.
 */
template<typename ElementType >
class VectorParameter: public efl::BasicVector<ElementType>,
  public TypedParameterBase<VectorParameterConfig, ParameterToId<VectorParameter<ElementType> >::id >
{
public:
  explicit VectorParameter(ParameterConfigBase const & config);

  explicit VectorParameter(VectorParameterConfig const & config);
};

} // namespace pml
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::pml::VectorParameter<float>, visr::ParameterType::VectorFloat, visr::pml::VectorParameterConfig )
DEFINE_PARAMETER_TYPE( visr::pml::VectorParameter<double>, visr::ParameterType::VectorDouble, visr::pml::VectorParameterConfig )

#endif // VISR_PML_VECTOR_PARAMETER_HPP_INCLUDED
