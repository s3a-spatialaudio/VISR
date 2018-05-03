/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_SPARSE_GAIN_MATRIX_HPP_INCLUDED
#define VISR_LIBRCL_SPARSE_GAIN_MATRIX_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/atomic_component.hpp>
#include <libvisr/audio_input.hpp>
#include <libvisr/audio_output.hpp>
#include <libvisr/constants.hpp>
#include <libvisr/parameter_input.hpp>

#include <librbbl/gain_fader.hpp>

#include <libpml/vector_parameter.hpp>
#include <libpml/double_buffering_protocol.hpp>
#include <libpml/message_queue_protocol.hpp>

#include <librbbl/sparse_gain_routing.hpp>

// #include <libefl/aligned_array.hpp>

#include <cstddef> // for std::size_t
#include <memory>
#include <valarray>

namespace visr
{

namespace rcl
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
class VISR_RCL_LIBRARY_SYMBOL SparseGainMatrix: public AtomicComponent
{
  using SampleType = visr::SampleType;
public:
  /**
   * Enumeration describing which control parameter input are to be instantiated.
   */
  enum class ControlInputs
  {
    No = 0,        ///< No control inputs
    Routings = 1,   ///< Set individual rpouting points (with gains)
    List = 2,       /// Reset the complete list of routing points.
    Gains = 4,      /// A vector of gains corresponding to the routing entries.
    All = Routings | List | Gains ///< Convenience value that activates all control inputs
  };

  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component
   * @param numberOfInputs The number of signals in the input signal.
   * @param numberOfOutputs The number of signals in the output signal.
   * @param interpolationSteps The number of samples needed for the transition after a new gain is set.
   * It must be an integral multiple of the period of the signal flow. The value "0" denotes an
   * immediate application of the new gain value.
   * @param initialGain The initial entries of the the gain matrix (linear scale). All entries are initialised to
   * this value (default: 0.0)
   * @param controlInputs Flag controlling whether to instantiate a parameter input to receive gain matrix updates.
   * @todo Describe the complete semantics of the transition.
   */
  explicit SparseGainMatrix( SignalFlowContext const & context,
                             char const * name,
                             CompositeComponent * parent,
                             std::size_t numberOfInputs,
                             std::size_t numberOfOutputs,
                             std::size_t interpolationSteps,
                             std::size_t maxRoutingPoints,
                             rbbl::SparseGainRoutingList const & initialRoutings = rbbl::SparseGainRoutingList(),
                             ControlInputs controlInputs = ControlInputs::No );

  void process( );

private:
  rbbl::GainFader< SampleType > mGainRamp;

  efl::BasicVector<SampleType> mOldGains;

  efl::BasicVector<SampleType> mNewGains;

  std::vector<std::size_t> mInputIndex;
  std::vector<std::size_t> mOutputIndex;

  std::vector<std::size_t> mRampIndex;

  AudioInput mInput;
  AudioOutput mOutput;

  using RoutingEntryInput = ParameterInput<pml::MessageQueueProtocol, pml::VectorParameter<SampleType> >;

  std::unique_ptr<ParameterInput<pml::DoubleBufferingProtocol, pml::VectorParameter<SampleType> > > mGainInput;
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_SPARSE_GAIN_MATRIX_HPP_INCLUDED
