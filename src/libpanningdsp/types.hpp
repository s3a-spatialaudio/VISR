/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBPANNINGDSP_TYPES_HPP_INCLUDED
#define VISR_LIBPANNINGDSP_TYPES_HPP_INCLUDED

#include <libefl/aligned_array.hpp>
#include <libefl/basic_matrix.hpp>

#include <libvisr/constants.hpp>
#include <libvisr/time.hpp>

#include <cstddef>
#include <limits>

namespace visr
{
namespace panningdsp
{

using SampleType = ::visr::SampleType;

/**
 * Type for representing time stamps.
 */
using TimeType = Time::IntegerTimeType;

/**
 * Type to represent the length of an gain interpolation.
 * @notr this is an unsigned type, i.e., intervals are supposed to be nonnegative.
 */
using InterpolationIntervalType = TimeType;

using TimeStampVector = visr::efl::AlignedArray< TimeType >;

using InterpolationIntervalVector = visr::efl::AlignedArray< InterpolationIntervalType >;

using GainMatrixType = visr::efl::BasicMatrix< SampleType >;

static constexpr std::size_t cTimeStampInfinity{ std::numeric_limits<TimeType>::max() };

} // namespace panningdsp
} // namespace visr

#endif // #ifndef VISR_LIBPANNINGDSP_TYPES_HPP_INCLUDED
