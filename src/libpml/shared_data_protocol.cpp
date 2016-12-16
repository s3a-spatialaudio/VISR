/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "shared_data_protocol.hpp"

#include "matrix_parameter.hpp"
#include "string_parameter.hpp"

#include <string>

namespace visr
{
namespace pml
{

// explicit instantiation
template class SharedDataProtocol<StringParameter>;
template class SharedDataProtocol<MatrixParameter<float> >;
template class SharedDataProtocol<MatrixParameter<double> >;

} // namespace pml
} // namespace visr
