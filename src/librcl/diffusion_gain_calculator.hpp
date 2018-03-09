/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_DIFFUSION_GAIN_CALCULATOR_HPP_INCLUDED
#define VISR_LIBRCL_DIFFUSION_GAIN_CALCULATOR_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/constants.hpp>
#include <libvisr/atomic_component.hpp>
#include <libvisr/parameter_input.hpp>
#include <libvisr/parameter_output.hpp>

#include <libobjectmodel/object.hpp> // needed basically for type definitions

#include <libpml/double_buffering_protocol.hpp>
#include <libpml/matrix_parameter.hpp>
#include <libpml/object_vector.hpp>
#include <libpml/shared_data_protocol.hpp>

#include <vector>

namespace visr
{
namespace rcl
{

/**
 * Audio component for extracting the diffuseness gain from an object vector.
 */
class VISR_RCL_LIBRARY_SYMBOL DiffusionGainCalculator: public AtomicComponent
{
public:
  /**
   * Type of the gain coefficients. We use the same type as the audio samples.
   */
  using CoefficientType = SampleType;

  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component
   * @param numberOfObjectChannels The number of object channels supported by this calculator.
   */
  explicit DiffusionGainCalculator( SignalFlowContext const & context,
                                    char const * name,
                                    CompositeComponent * parent,
                                    std::size_t numberOfObjectChannels );

  /**
   * Disabled (deleted) copy constructor
   */
  DiffusionGainCalculator( DiffusionGainCalculator const & ) = delete;


  /**
   * Destructor.
   */
  ~DiffusionGainCalculator();

//  /**
//   * Method to initialise the component.
//   */
//  void setup( std::size_t numberOfObjectChannels );

  /**
  * The process function.
  */
  void process() override;

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
  ParameterOutput< pml::SharedDataProtocol, pml::MatrixParameter<CoefficientType > > mGainOutput;

};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_DIFFUSION_GAIN_CALCULATOR_HPP_INCLUDED
