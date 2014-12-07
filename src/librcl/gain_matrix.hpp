/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_GAIN_MATRIX_HPP_INCLUDED
#define VISR_LIBRCL_GAIN_MATRIX_HPP_INCLUDED

#include <libril/audio_component.hpp>

namespace visr
{
namespace rcl
{

/**
 * Audio Component for matrixing a vector of input signals to a vector
 * of output signals with a specific gain value for each routing
 * point.
 * The gain matrix can be changed at runtime. Optionally, the class
 * features smooth transistions between differing gain settings.
 */
class GainMatrix: public ril::AudioComponent
{
public:
private:
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_GAIN_MATRIX_HPP_INCLUDED
