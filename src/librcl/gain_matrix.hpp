/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_GAIN_MATRIX_HPP_INCLUDED
#define VISR_LIBRCL_GAIN_MATRIX_HPP_INCLUDED

#include <libril/audio_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/constants.hpp>

// TODO: make it a forward declaration
#include <librbbl/gain_matrix.hpp>

// For some reason, the forward declaration causes a compile error on MSVC,
// so we include the header for the moment.
// Also, I am not sure whether it makes sense to use a separate type as an alias to efl::BasicMatrix
#include <libpml/matrix_parameter.hpp>

#include <libefl/aligned_array.hpp>


#include <cstddef> // for std::size_t

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
class GainMatrix: public ril::AudioComponent
{
  using SampleType = ril::SampleType;
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit GainMatrix( ril::AudioSignalFlow& container, char const * name );
    
  /**
   * Setup method to initialise the object and set the parameters.
   * @param numberOfInputs The number of signals in the input signal.
   * @param numberOfOutputs The number of signals in the output signal.
   * @param interpolationSteps The number of samples needed for the transition after a new gain is set.
   * It must be an integral multiple of the period of the signal flow. The value "0" denotes an
   * immediate application of the new gain value.
   * @param initialGain The initial entries of the the gain matrix (linear scale). All entries are initialised to 
   * this value (default: 0.0)
   * @todo Describe the complete semantics of the transition.
   */
  void setup( std::size_t numberOfInputs, 
              std::size_t numberOfOutputs,
              std::size_t interpolationSteps,
              SampleType initialGain = static_cast<SampleType>(0.0) );
  /**
  * Setup method to initialise the object and set the parameters.
  * @param numberOfInputs The number of signals in the input signal.
  * @param numberOfOutputs The number of signals in the output signal.
  * @param interpolationSteps The number of samples needed for the transition after a new gain is set.
  * It must be an integral multiple of the period of the signal flow. The value "0" denotes an
  * immediate application of the new gain value.
  * @param initialMatrix The initial entries of the the gain matrix (linear scale). The row and column 
  * numbers of the matrix must match the arguments numberOfOutputs and numberOfInputs, respectively.
  */
  void setup( std::size_t numberOfInputs,
              std::size_t numberOfOutputs,
              std::size_t interpolationSteps,
              efl::BasicMatrix< SampleType > const & initialGains );

  void process( );

  void setGains( efl::BasicMatrix< SampleType > const & newGains );

private:
  std::unique_ptr< rbbl::GainMatrix< SampleType > > mMatrix;

  ril::AudioInput mInput;
  ril::AudioOutput mOutput;

  /**
   * 
   */
  std::size_t mNumberOfInputs;
  std::size_t mNumberOfOutputs;
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_GAIN_MATRIX_HPP_INCLUDED
