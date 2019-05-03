/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_GAIN_MATRIX_HPP_INCLUDED
#define VISR_LIBRCL_GAIN_MATRIX_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/atomic_component.hpp>
#include <libvisr/audio_input.hpp>
#include <libvisr/audio_output.hpp>
#include <libvisr/constants.hpp>
#include <libvisr/parameter_input.hpp>

// TODO: make it a forward declaration
#include <librbbl/gain_matrix.hpp>

// For some reason, the forward declaration causes a compile error on MSVC,
// so we include the header for the moment.
// Also, I am not sure whether it makes sense to use a separate type as an alias to efl::BasicMatrix
#include <libpml/matrix_parameter.hpp>
#include <libpml/shared_data_protocol.hpp>

#include <libefl/aligned_array.hpp>


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
class VISR_RCL_LIBRARY_SYMBOL GainMatrix: public AtomicComponent
{
  using SampleType = visr::SampleType;
public:
  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component
   */
  explicit GainMatrix( SignalFlowContext const & context,
                       char const * name,
                       CompositeComponent * parent = nullptr );
    
  /**
   * Setup method to initialise the object and set the parameters.
   * @param numberOfInputs The number of signals in the input signal.
   * @param numberOfOutputs The number of signals in the output signal.
   * @param interpolationSteps The number of samples needed for the transition after a new gain is set.
   * It must be an integral multiple of the period of the signal flow. The value "0" denotes an
   * immediate application of the new gain value.
   * @param initialGain The initial entries of the the gain matrix (linear scale). All entries are initialised to 
   * this value (default: 0.0)
   * @param controlInput Flag controlling whether to instantiate a parameter input to receive gain matrix updates.
   * @todo Describe the complete semantics of the transition.
   */
  void setup( std::size_t numberOfInputs, 
              std::size_t numberOfOutputs,
              std::size_t interpolationSteps,
              SampleType initialGain = static_cast<SampleType>(0.0),
              bool controlInput = true );
  /**
  * Setup method to initialise the object and set the parameters.
  * @param numberOfInputs The number of signals in the input signal.
  * @param numberOfOutputs The number of signals in the output signal.
  * @param interpolationSteps The number of samples needed for the transition after a new gain is set.
  * It must be an integral multiple of the period of the signal flow. The value "0" denotes an
  * immediate application of the new gain value.
  * @param initialGains The initial entries of the the gain matrix (linear scale). The row and column
  * numbers of the matrix must match the arguments numberOfOutputs and numberOfInputs, respectively.
  * @param controlInput Flag controlling whether to instantiate a parameter input to receive gain matrix updates.
  */
  void setup( std::size_t numberOfInputs,
              std::size_t numberOfOutputs,
              std::size_t interpolationSteps,
              efl::BasicMatrix< SampleType > const & initialGains,
              bool controlInput = true );

  void process( );

private:
  std::unique_ptr< rbbl::GainMatrix< SampleType > > mMatrix;

  AudioInput mInput;
  AudioOutput mOutput;

  /**
  * Vectors to the channel pointers of the input and output ports.
  * They are required by the interface of the contained rbbl::GainMatrix class.
  * @todo Consider (optional) stride-based interface for rbbl::GainMatrix.
  */
  //@{
  std::valarray<SampleType const *> mInputChannels;
  std::valarray<SampleType * > mOutputChannels;
  //@}

  std::unique_ptr<ParameterInput<pml::SharedDataProtocol, pml::MatrixParameter<SampleType> > > mGainInput;
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_GAIN_MATRIX_HPP_INCLUDED
