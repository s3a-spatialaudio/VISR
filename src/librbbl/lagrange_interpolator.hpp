/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_LAGRANGE_INTERPOLATOR_HPP_INCLUDED
#define VISR_LIBRBBL_LAGRANGE_INTERPOLATOR_HPP_INCLUDED

#include "fractional_delay_base.hpp"

#include <libefl/basic_vector.hpp>
#include <libefl/lagrange_coefficient_calculator.hpp>

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

  /**
   * Constructor, initialises internal data structures.
   * @param maxNumSamples The maximum number o samples that shall be processed in one call to interpolate().
   */
  explicit LagrangeInterpolator( std::size_t maxNumSamples,
                                 std::size_t alignmentElements = 0);

  virtual ~LagrangeInterpolator();

  SampleType methodDelay() const override;

  virtual void interpolate( SampleType const * basePointer,
                            SampleType * result,
                            std::size_t numSamples,
                            SampleType startDelay, SampleType endDelay,
                            SampleType startGain, SampleType endGain ) override;
private:

  std::vector<std::ptrdiff_t> mBaseOffsets;

  efl::BasicVector<SampleType> mDelays;

  efl::BasicVector<SampleType> mGains;

  efl::BasicVector<SampleType> mIntersamplePositions;

  efl::LagrangeCoefficientCalculator<SampleType, order, true> const mCoeffCalculator;

  /**
   * @note a special case is needed for order == 0 (nearest sample interpolation to avoid underflow and access to future values.
   */
  static constexpr SampleType cMethodDelay = static_cast<SampleType>(0.5) * static_cast<SampleType>(order);
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_LAGRANGE_INTERPOLATOR_HPP_INCLUDED
