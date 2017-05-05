/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_CONSTANTS_HPP_INCLUDED
#define VISR_CONSTANTS_HPP_INCLUDED

#include <cstddef>

namespace visr
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
 * Desired alignment for contiguous vectors of the default sample type, given in number of elements o.
 */
static const std::size_t cVectorAlignmentSamples = cVectorAlignmentBytes / sizeof(SampleType);

} // namespace visr

#endif // #ifndef VISR_CONSTANTS_HPP_INCLUDED
