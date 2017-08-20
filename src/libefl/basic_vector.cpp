/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "basic_vector.hpp"

#include <complex>

namespace visr
{
namespace efl
{

template class BasicVector<float>;
template class BasicVector<double>;
template class BasicVector<std::complex<float> >;
template class BasicVector<std::complex<double> >;

} // namespace efl
} // namespace visr
