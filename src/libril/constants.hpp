/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_CONSTANTS_HPP_INCLUDED
#define VISR_LIBRIL_CONSTANTS_HPP_INCLUDED

#include <cstddef>

namespace visr
{
namespace ril
{

/**
 * The data type used for transferring audio samples within the system.
 */
using SampleType = float;

/**
 * The data type for determining the sampling frequency.
 */  
using SamplingFrequencyType = std::size_t;
  
/**
 * Desired alignment for contiguous vectors, given in byte.
 * The chosen value (64) follows the guidelines for modern Intel processors.
 * Adjust for other architectures as needed.
 */
static const std::size_t cVectorAlignmentBytes = 64;

/**
 * Desired alignement for contiguous vectors, given in byte.
 */
static const std::size_t cVectorAlignmentSamples = cVectorAlignmentBytes / sizeof(SampleType);

} // namespace ril
} // namespace constants

#endif // #ifndef VISR_LIBRIL_CONSTANTS_HPP_INCLUDED
