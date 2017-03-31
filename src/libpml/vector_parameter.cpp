/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "vector_parameter.hpp"

#include <libril/constants.hpp>
#include <libril/parameter_factory.hpp>

namespace visr
{
namespace pml
{

template< typename ElementType >
VectorParameter<ElementType>::VectorParameter( ParameterConfigBase const & config )
: VectorParameter( dynamic_cast<VectorParameterConfig const &>(config) )
{
}

template< typename ElementType >
VectorParameter<ElementType>::VectorParameter( VectorParameterConfig const & config )
: efl::BasicVector<ElementType>( config.numberOfElements(), cVectorAlignmentSamples ) // Use standard alignment
{
}

// Explicit instantiations for element types float and double and the corresponding complex types.
template class VectorParameter<float>;
template class VectorParameter<double>;
template class VectorParameter<std::complex<float> >;
template class VectorParameter<std::complex<double> >;

} // namespace pml
} // namespace visr
