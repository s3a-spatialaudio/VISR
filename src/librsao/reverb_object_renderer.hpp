/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_RSAO_REVERB_OBJECT_RENDERER_HPP_INCLUDED
#define VISR_RSAO_REVERB_OBJECT_RENDERER_HPP_INCLUDED

#include <libril/composite_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/parameter_input.hpp>

#include <librcl/add.hpp>
#include <librcl/biquad_iir_filter.hpp>
#include <librcl/delay_vector.hpp>
#include <librcl/fir_filter_matrix.hpp>
#include <librcl/gain_matrix.hpp>
#include <librcl/signal_routing.hpp>

#include "late_reverb_filter_calculator.hpp"
#include "reverb_parameter_calculator.hpp"

#include <libefl/basic_matrix.hpp>

#include <libpml/listener_position.hpp>
#include <libpml/double_buffering_protocol.hpp>
#include <libpml/object_vector.hpp>

#include <memory>
#include <string>

namespace visr
{
namespace rsao
{

/**
 * Audio signal graph object for the VISR baseline renderer.
 */
class ReverbObjectRenderer: public CompositeComponent
{
public:
  /**
   * Constructor to create, initialise and interconnect all processing components.
   */
  explicit ReverbObjectRenderer( SignalFlowContext const & context,
                                 char const * name,
                                 CompositeComponent * parent,
                                 std::string const & reverbConfig,
                                 panning::LoudspeakerArray const & arrayConfig, 
                                 std::size_t numberOfObjectSignals );

  ~ReverbObjectRenderer();

private:
  AudioInput mObjectSignalInput;

  AudioOutput mLoudspeakerOutput;

  ParameterInput< pml::DoubleBufferingProtocol, pml::ObjectVector > mObjectVector;
   
  rsao::ReverbParameterCalculator mReverbParameterCalculator;

  rcl::SignalRouting mReverbSignalRouting;

  rcl::DelayVector mDiscreteReverbDelay;

  rcl::BiquadIirFilter mDiscreteReverbReflFilters;

  rcl::GainMatrix mDiscreteReverbPanningMatrix;

  rsao::LateReverbFilterCalculator mLateReverbFilterCalculator;

  /**
   * Overall gain and delay for the source signals going into the late
   * reverberation part.
   * This is used to apply the object level, but should also apply the
   * onset delay.
   */
  rcl::DelayVector mLateReverbGainDelay;

  rcl::FirFilterMatrix mLateReverbFilter;

  rcl::FirFilterMatrix mLateDiffusionFilter;

  rcl::Add mReverbMix;
};

} // namespace rsao
} // namespace visr

#endif // VISR_RSAO_REVERB_OBJECT_RENDERER_HPP_INCLUDED
