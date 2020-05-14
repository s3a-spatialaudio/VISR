/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBPANNINGDSP_PANNING_GAIN_MATRIX_HPP_INCLUDED
#define VISR_LIBPANNINGDSP_PANNING_GAIN_MATRIX_HPP_INCLUDED

#include "export_symbols.hpp"
#include "panning_matrix_parameter.hpp"

#include <libvisr/atomic_component.hpp>
#include <libvisr/audio_input.hpp>
#include <libvisr/audio_output.hpp>
#include <libvisr/constants.hpp>
#include <libvisr/parameter_input.hpp>

#include <libpml/double_buffering_protocol.hpp>

#include <libefl/basic_matrix.hpp>
#include <libefl/basic_vector.hpp>

#include <cstddef> // for std::size_t
#include <memory>
#include <valarray>

namespace visr
{
namespace panningdsp
{

/**
 * Audio Component for matrixing a vector of input signals to a vector
 * of output signals with a specific gain value for each routing
 * point.
 * The gain matrix can be changed at runtime. Optionally, the class
 * features smooth transitions between differing gain settings.
 * This class has one input port named "in" and one output port named "out".
 * The width of these ports is determined by the arguments "numberOfInput" and "numberOfOutputs", respectively,
 * which are passed to the setup() method.
 */
class VISR_PANNINGDSP_LIBRARY_SYMBOL PanningGainMatrix: public AtomicComponent
{
public:
  using SampleType = visr::SampleType;

  PanningGainMatrix(SignalFlowContext const & context,
    char const * name,
    CompositeComponent * parent,
    std::size_t numberOfObjects,
    std::size_t numberOfLoudspeakers );

  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component
   */
  PanningGainMatrix(SignalFlowContext const & context,
    char const * name,
    CompositeComponent * parent,
    std::size_t numberOfObjects,
    std::size_t numberOfLoudspeakers,
    efl::BasicMatrix<SampleType> const & initialGains);

  PanningGainMatrix(SignalFlowContext const & context,
    char const * name,
    CompositeComponent * parent,
    std::size_t numberOfObjects,
    std::size_t numberOfLoudspeakers,
    efl::BasicMatrix<SampleType> const & initialGains,
    PanningMatrixParameter const & initialSlope );

  ~PanningGainMatrix() override;

  void process() override;

private:

  void processParameters();

  void processAudio();

  /**
   * Process the audio signal for a single 'slope', that is, a number 
   * of samples with no change if the gain increase. 
   */
  void processAudioSingleSlope( std::size_t objIdx, TimeType currentTime,
    std::size_t startIdx, std::size_t duration,
    bool accumulate, std::size_t alignment );

  void updateSlopeParameters( PanningMatrixParameter const & newParams );

  void updateSlopeParameter( std::size_t objIndex,
                             TimeType startTime,
                             InterpolationIntervalType duration,
                             SampleType const * gains );

  void scaleSignal( SampleType const * input, SampleType * output,
    std::size_t startIdx, std::size_t duration,
    SampleType gainStart, SampleType gainInc,
    bool accumulate, std::size_t alignment );

  AudioInput mAudioInput;
  AudioOutput mAudioOutput;

  using GainInput = ParameterInput< pml::DoubleBufferingProtocol, PanningMatrixParameter >; 

  GainInput mGainInput;

  TimeStampVector mPreviousTime;
  TimeStampVector mCurrentTargetTime;
  TimeStampVector mNextTargetTime;

  visr::efl::BasicMatrix< SampleType > mPreviousGains;
  visr::efl::BasicMatrix< SampleType > mCurrentTargetGains;
  visr::efl::BasicMatrix< SampleType > mNextTargetGains;

  visr::efl::BasicVector< SampleType > mScalingRamp;
};

} // namespace panningdsp
} // namespace visr

#endif // #ifndef VISR_LIBPANNINGDSP_PANNING_GAIN_MATRIX_HPP_INCLUDED
