/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "lagrange_interpolator.hpp"

#include <libril/detail/compose_message_string.hpp>

#include <array>
#include <cmath>
#include <numeric>

namespace visr
{
namespace rbbl
{
template <typename SampleType, std::size_t order >
LagrangeInterpolator<SampleType, order >::LagrangeInterpolator( std::size_t maxNumSamples,
                                                                std::size_t alignmentElements )
  : mBaseOffsets( maxNumSamples )
  , mDelays(  maxNumSamples, alignmentElements )
  , mGains( maxNumSamples, alignmentElements )
  , mIntersamplePositions( maxNumSamples, alignmentElements )
{
}

template <typename SampleType, std::size_t order >
LagrangeInterpolator<SampleType, order>::~LagrangeInterpolator() = default;

template <typename SampleType, std::size_t order >
SampleType LagrangeInterpolator<SampleType, order>::methodDelay() const
{
  return cMethodDelay;
}

template <typename SampleType, std::size_t order >
void LagrangeInterpolator<SampleType, order>::interpolate( SampleType const * basePointer,
                                                           SampleType * result,
                                                           std::size_t numSamples,
                                                           SampleType startDelay, SampleType endDelay,
                                                           SampleType startGain, SampleType endGain )
{
  if( numSamples > mDelays.size() )
  {
    throw std::invalid_argument( "LagrangeInterpolator::interpolate(): number of elements exceeds maximum admissible number.");
  }

  efl::ErrorCode res = efl::vectorRamp( mGains.data(), numSamples,
                                        startGain, endGain,
                                        false/*startInclusive*/, true/*endInclusive*/,
                                        mGains.alignmentElements() );
  if( res != efl::noError )
  {
    throw std::runtime_error( detail::composeMessageString( "LagrangeInterpolator::interpolate(): Error during gain ramp computation:",
                                                            efl::errorMessage(res)) );
  }

  SampleType const adjustedStartTimeOffset = -static_cast<SampleType>(numSamples + order) - startDelay;
  SampleType const adjustedEndTimeOffset = -static_cast<SampleType>(order) - endDelay;

  res = efl::vectorRamp( mDelays.data(), numSamples,
                         adjustedStartTimeOffset,
                         adjustedEndTimeOffset,
                         false/*startInclusive*/, true/*endInclusive*/,
                         mDelays.alignmentElements() );
  if( res != efl::noError )
  {
    throw std::runtime_error( detail::composeMessageString( "LagrangeInterpolator::interpolate(): Error during delay ramp computation:",
                                                            efl::errorMessage(res)) );
  }
  // This needs to be replaces by efl functions in time (round and integer conversion functions are costly in the default implementation.
  // std::round always rounds away from zero
  std::transform( mDelays.data(), mDelays.data() + numSamples, mIntersamplePositions.data(),
                  []( SampleType val ){ return std::round( val ); } );
  std::transform( mIntersamplePositions.data(), mIntersamplePositions.data() + numSamples, mBaseOffsets.data(),
                  []( SampleType val ){ return static_cast<std::ptrdiff_t>(val); } );

  res = efl::vectorSubtractInplace( mDelays.data(), mIntersamplePositions.data(), numSamples,
                                    mIntersamplePositions.alignmentElements() );
  if( res != efl::noError )
  {
    throw std::runtime_error( detail::composeMessageString( "LagrangeInterpolator::interpolate(): Error computing intersample positions.",
                                                            efl::errorMessage(res)) );
  }

  std::array<SampleType, order+1> lagrangeCoefficients;
  for( std::size_t sampleIdx( 0 ); sampleIdx < numSamples; ++sampleIdx )
  {
    mCoeffCalculator.calculateCoefficients( mIntersamplePositions[sampleIdx],
                                            &lagrangeCoefficients[0] );
    result[sampleIdx] = mGains[sampleIdx] * std::inner_product( lagrangeCoefficients.begin(), lagrangeCoefficients.end(),
                                                                basePointer + mBaseOffsets[sampleIdx],
                                                                static_cast<SampleType>(0.0) );
  }
}

// Explicit instantiations
template class LagrangeInterpolator<float, 0>;
template class LagrangeInterpolator<float, 1>;
template class LagrangeInterpolator<float, 2>;
template class LagrangeInterpolator<float, 3>;
template class LagrangeInterpolator<float, 4>;
template class LagrangeInterpolator<float, 5>;
template class LagrangeInterpolator<float, 6>;
template class LagrangeInterpolator<float, 7>;

template class LagrangeInterpolator<double, 0>;
template class LagrangeInterpolator<double, 1>;
template class LagrangeInterpolator<double, 2>;
template class LagrangeInterpolator<double, 3>;
template class LagrangeInterpolator<double, 4>;
template class LagrangeInterpolator<double, 5>;
template class LagrangeInterpolator<double, 6>;
template class LagrangeInterpolator<double, 7>;

} // namespace rbbl
} // namespace visr
