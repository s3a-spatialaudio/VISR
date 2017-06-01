/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "lagrange_interpolator.hpp"

namespace visr
{
namespace rbbl
{
template <typename SampleType, std::size_t order >
LagrangeInterpolator<SampleType, order >::LagrangeInterpolator( std::size_t maxNumSamples )
  : mBaseOffsets( maxNumSamples )
  , mIntersamplePositions( maxNumSamples, 0 ) // TODO: Adjust alignment.
{
}

template <typename SampleType, std::size_t order >
LagrangeInterpolator<SampleType, order>::~LagrangeInterpolator() = default;

template <typename SampleType, std::size_t order >
void LagrangeInterpolator<SampleType, order>::interpolate( SampleType const * basePointer,
                                                           SampleType * result,
                                                           std::size_t numSamples,
                                                           SampleType startDelay, SampleType endDelay,
                                                           SampleType startGain, SampleType endGain )
{
}

} // namespace rbbl
} // namespace visr
