/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_LAGRANGE_INTERPOLATOR_HPP_INCLUDED
#define VISR_LIBRBBL_LAGRANGE_INTERPOLATOR_HPP_INCLUDED

#include "fractional_delay_base.hpp"

#include <libefl/lagrange_interpolator.hpp>
#include <libefl/basic_vector.hpp>

#include <cstddef>
#include <vector>

namespace visr
{
namespace rbbl
{

/**
 * Abstract base class for interpolation algorithms on discrete-time sequences.
 */
template <typename SampleType, std::size_t order >
class LagrangeInterpolator: public FractionalDelayBase<SampleType>
{
public:
  explicit LagrangeInterpolator( std::size_t maxNumSamples );

  virtual ~LagrangeInterpolator();

  SampleType methodDelay() const override;

  virtual void interpolate( SampleType const * basePointer,
                            SampleType * result,
                            std::size_t numSamples,
                            SampleType startDelay, SampleType endDelay,
                            SampleType startGain, SampleType endGain ) override;
  
  std::vector<std::ptrdiff_t> mBaseOffsets;

  efl::BasicVector<SampleType> mIntersamplePositions;

  efl::BasicVector<SampleType> mGains;

  efl::LagrangeInterpolator<SampleType, order> mCoeffCalculator;
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_LAGRANGE_INTERPOLATOR_HPP_INCLUDED
