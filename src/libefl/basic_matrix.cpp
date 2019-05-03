/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "basic_matrix.hpp"

#include <complex>

namespace visr
{
namespace efl
{

template class BasicMatrix<float>;
template class BasicMatrix<double>;
template class BasicMatrix<std::complex<float> >;
template class BasicMatrix<std::complex<double> >;

} // namespace efl
} // namespace visr
