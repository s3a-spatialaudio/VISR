/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "fractional_delay_base.hpp"

namespace visr
{
namespace rbbl
{

template <typename SampleType>
FractionalDelayBase<SampleType>::~FractionalDelayBase() = default;


// Explicit instantiations
template class FractionalDelayBase<float>;
template class FractionalDelayBase<double>;


} // namespace rbbl
} // namespace visr
