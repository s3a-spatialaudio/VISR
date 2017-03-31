/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_DIFFUSION_GAIN_CALCULATOR_HPP_INCLUDED
#define VISR_LIBRCL_DIFFUSION_GAIN_CALCULATOR_HPP_INCLUDED

#include <libril/constants.hpp>
#include <libril/atomic_component.hpp>
#include <libril/parameter_input.hpp>
#include <libril/parameter_output.hpp>

#include <libobjectmodel/object.hpp> // needed basically for type definitions

#include <libpml/double_buffering_protocol.hpp>
#include <libpml/matrix_parameter.hpp>
#include <libpml/object_vector.hpp>
#include <libpml/shared_data_protocol.hpp>

#include <memory>
#include <vector>

namespace visr
{
namespace rcl
{

/**
 * Audio component for extracting the diffuseness gain from an object vector.
 */
class DiffusionGainCalculator: public AtomicComponent
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
  explicit DiffusionGainCalculator( SignalFlowContext& context,
                                    char const * name,
                                    CompositeComponent * parent = nullptr );

  /**
   * Disabled (deleted) copy constructor
   */
  DiffusionGainCalculator( DiffusionGainCalculator const & ) = delete;


  /**
   * Destructor.
   */
  ~DiffusionGainCalculator();

  /**
   * Method to initialise the component.
   * @param numberOfObjectChannels The number of object channels supported by this calculator.
   */ 
  void setup( std::size_t numberOfObjectChannels );

  /**
  * The process function.
  * It takes a vector of objects as input and calculates a vector of output gains.
  * @param objects The vector of objects. It must consist only of single-channel objects with channel IDs 0...numberOfChannelObjects-1.
  * @param[out] gainVector The vector of diffusion gains for the audio channels. Must be a vector of size numberOfChannelObjects.
  */
  void process();

private:
  /**
   * The number of audio object channels handled by this object.
   */
  std::size_t mNumberOfObjectChannels;

  /**
   * Internal implementation method for the common part of both process() function.
   */
  void processInternal( objectmodel::ObjectVector const & objects, CoefficientType * gains );

  ParameterInput< pml::DoubleBufferingProtocol, pml::ObjectVector > mObjectVectorInput;
  std::unique_ptr<ParameterOutput< pml::SharedDataProtocol, pml::MatrixParameter<CoefficientType > > > mGainOutput;

};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_DIFFUSION_GAIN_CALCULATOR_HPP_INCLUDED
