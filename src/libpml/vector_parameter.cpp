/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "vector_parameter.hpp"

#include <libril/constants.hpp>

namespace visr
{
namespace pml
{


template< typename ElementType >
VectorParameter<ElementType>::VectorParameter( ril::ParameterConfigBase const & config )
: VectorParameter( dynamic_cast<VectorParameterConfig const &>(config) )
{
}

template< typename ElementType >
VectorParameter<ElementType>::VectorParameter( VectorParameterConfig const & config )
: efl::BasicVector<ElementType>( config.numberOfElements(), ril::cVectorAlignmentSamples ) // Use standard alignment
{
}

// Explicit instantiations for element types float and double
template class VectorParameter<float>;
template class VectorParameter<double>;

} // namespace pml
} // namespace visr
