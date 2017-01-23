/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_OBJECT_GAIN_EQ_CALCULATOR_HPP_INCLUDED
#define VISR_LIBRCL_OBJECT_GAIN_EQ_CALCULATOR_HPP_INCLUDED

#include <libril/audio_component.hpp>

// #include <libobjectmodel/object.hpp> // needed basically for type definitions

#include <memory>

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
// template< typename SampleType > class BiquadParameterList;
}
  
namespace rcl
{

/**
 * Audio component for extracting the diffuseness gain from an object vector.
 */
class ObjectGainEqCalculator: public ril::AudioComponent
{
public:
  /**
   * Type of the gain coefficients. We use the same type as the audio samples.
   */
  using CoefficientType = ril::SampleType;

  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit ObjectGainEqCalculator( ril::AudioSignalFlow& container, char const * name );

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
   */ 
  void setup( std::size_t numberOfObjectChannels,
              std::size_t numberOfBiquadSections);

  /**
   * The process function.
   * It takes an ObjectVector as input and calculates a vector of output gains and a matrix of EQ settings.
   * @param objects The vector of objects. It must consist only of single-channel objects with channel IDs 0...numberOfChannelObjects-1.
   * @param[out] objectSignalGains A parameter vector to be filled with the object gains (linear scale), dimension: numberOfObjectChannels
   * @param[out] objectChannelEqs Matrix object to be filled with EQ settings encoded in the audio object.
   */
  void process( objectmodel::ObjectVector const & objects,
                efl::BasicVector<CoefficientType> & objectSignalGains,
                pml::BiquadParameterMatrix<CoefficientType> & objectChannelEqs );

private:
  /**
   * The number of audio object channels handled by this object.
   */
  std::size_t mNumberOfObjectChannels;

  std::size_t mNumberOfBiquadSections;
    
  /**
   *
   */
  ril::SamplingFrequencyType const cSamplingFrequency;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_OBJECT_GAIN_EQ_CALCULATOR_HPP_INCLUDED
