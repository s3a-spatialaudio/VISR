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
#include <librbbl/sparse_gain_routing.hpp>

#include <libpml/double_buffering_protocol.hpp>
#include <libpml/message_queue_protocol.hpp>
#include <libpml/shared_data_protocol.hpp>
#include <libpml/vector_parameter.hpp>
#include <libpml/sparse_gain_routing_parameter.hpp>

#include <memory>
#include <vector>

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
  enum class ControlPortConfig
  {
    No = 0,            ///< No control inputs
    RoutingPoints = 1, ///< Set individual routing points (with gains)
    RoutingList = 2,   ///< Reset the complete list of routing points.
    Gain = 4,          ///< A vector of gains corresponding to the routing entries.
    All = RoutingPoints | RoutingList | Gain ///< Convenience value that activates all control inputs
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
                             ControlPortConfig controlInputs = ControlPortConfig::No );

  void process( ) override;

private:
  void setRoutings( rbbl::SparseGainRoutingList const & newRoutings );

  efl::BasicVector<SampleType> mPreviousGains;

  efl::BasicVector<SampleType> mNextGains;

  rbbl::SparseGainRoutingList mRoutings;

  std::size_t mRampIndex;

  std::size_t const mNumRoutingPoints;

  rbbl::GainFader<SampleType> const mGainRamp;

  AudioInput mInput;
  AudioOutput mOutput;

  using GainInput = ParameterInput<pml::DoubleBufferingProtocol, pml::VectorParameter<SampleType> >;
  using RoutingPointInput = ParameterInput<pml::MessageQueueProtocol, pml::SparseGainRoutingParameter >;
  using RoutingListInput = ParameterInput<pml::DoubleBufferingProtocol, pml::SparseGainRoutingListParameter >;

  std::unique_ptr< GainInput > mGainInput;

  std::unique_ptr< RoutingPointInput > mRoutingPointInput;

  std::unique_ptr< RoutingListInput > mRoutingListInput;
};

/**
* Bitwise operator to combine control input flags.
*/
VISR_RCL_LIBRARY_SYMBOL SparseGainMatrix::ControlPortConfig operator|( SparseGainMatrix::ControlPortConfig lhs,
  SparseGainMatrix::ControlPortConfig rhs );

/**
* Bitwise operator to extract mask control input flags.
*/
VISR_RCL_LIBRARY_SYMBOL SparseGainMatrix::ControlPortConfig operator&( SparseGainMatrix::ControlPortConfig lhs,
  SparseGainMatrix::ControlPortConfig rhs );

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_SPARSE_GAIN_MATRIX_HPP_INCLUDED
