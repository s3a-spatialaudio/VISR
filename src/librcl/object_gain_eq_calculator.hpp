/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_OBJECT_GAIN_EQ_CALCULATOR_HPP_INCLUDED
#define VISR_LIBRCL_OBJECT_GAIN_EQ_CALCULATOR_HPP_INCLUDED

#include <libril/atomic_component.hpp>
#include <libril/parameter_input.hpp>
#include <libril/parameter_output.hpp>

#include <memory>

#include <libpml/double_buffering_protocol.hpp>
#include <libpml/biquad_parameter.hpp>
#include <libpml/object_vector.hpp>
#include <libpml/vector_parameter.hpp>

namespace visr
{
// forward declarations
namespace objectmodel
{
class ObjectVector;
}
namespace efl
{
template< typename SampleType > class BasicVector;
}
namespace pml
{
template< typename SampleType > class BiquadParameterMatrix;
}
  
namespace rcl
{

/**
 * Audio component for extracting the diffuseness gain from an object vector.
 */
class ObjectGainEqCalculator: public AtomicComponent
{
public:
  /**
   * Type of the gain coefficients. We use the same type as the audio samples.
   */
  using CoefficientType = SampleType;

  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit ObjectGainEqCalculator( SignalFlowContext const & context,
                                   char const * name,
                                   CompositeComponent * parent );

  /**
   * Disabled (deleted) copy constructor
   */
  ObjectGainEqCalculator( ObjectGainEqCalculator const & ) = delete;


  /**
   * Destructor.
   */
  ~ObjectGainEqCalculator();

  /**
   * Method to initialise the component.
   * @param numberOfObjectChannels The number of object channels supported by this calculator.
   * @param numberOfBiquadSections The number of separate biquad filter sections for each audio channel.
   */ 
  void setup( std::size_t numberOfObjectChannels,
              std::size_t numberOfBiquadSections);

  void process( ) override;

private:
  /**
   * Internal process function.
   * It takes an ObjectVector as input and calculates a vector of output gains and a matrix of EQ settings.
   * @param objects The vector of objects. It must consist only of single-channel objects with channel IDs 0...numberOfChannelObjects-1.
   * @param[out] objectSignalGains A parameter vector to be filled with the object gains (linear scale), dimension: numberOfObjectChannels
   * @param[out] objectChannelEqs Matrix object to be filled with EQ settings encoded in the audio object.
   */
  void process( objectmodel::ObjectVector const & objects,
                efl::BasicVector<CoefficientType> & objectSignalGains,
                pml::BiquadParameterMatrix<CoefficientType> & objectChannelEqs );

  ParameterInput<pml::DoubleBufferingProtocol, pml::ObjectVector> mObjectInput;
  ParameterOutput<pml::DoubleBufferingProtocol, pml::VectorParameter<CoefficientType> > mGainOutput;
  ParameterOutput<pml::DoubleBufferingProtocol, pml::BiquadParameterMatrix<CoefficientType> > mEqOutput;

  /**
   * The number of audio object channels handled by this object.
   */
  std::size_t mNumberOfObjectChannels;

  std::size_t mNumberOfBiquadSections;
    
  /**
   *
   */
  SamplingFrequencyType const cSamplingFrequency;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_OBJECT_GAIN_EQ_CALCULATOR_HPP_INCLUDED
