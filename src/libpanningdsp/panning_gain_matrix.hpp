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

#include <libefl/aligned_array.hpp>

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

  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component
   */
  PanningGainMatrix::PanningGainMatrix(SignalFlowContext const & context,
    char const * name,
    CompositeComponent * parent,
    std::size_t numberOfObjects,
    std::size_t numberOfLoudspeakers,
    efl::BasicMatrix<SampleType> const & initialGains);

  ~PanningGainMatrix() override;

  void process() override;

private:

  void processParameters();

  void processAudio();

  AudioInput mAudioInput;
  AudioOutput mAudioOutput;

  // std::unique_ptr<ParameterInput<pml::SharedDataProtocol, pml::MatrixParameter<SampleType> > > mGainInput;
};

} // namespace panningdsp
} // namespace visr

#endif // #ifndef VISR_LIBPANNINGDSP_PANNING_GAIN_MATRIX_HPP_INCLUDED
