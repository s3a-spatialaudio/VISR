/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_HOA_ALLRAP_GAIN_CALCULATOR_HPP_INCLUDED
#define VISR_LIBRCL_HOA_ALLRAP_GAIN_CALCULATOR_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/constants.hpp>
#include <libvisr/atomic_component.hpp>
#include <libvisr/parameter_input.hpp>
#include <libvisr/parameter_output.hpp>

#include <libobjectmodel/object.hpp> // needed basically for type definitions

#include <libpanning/AllRAD.h>
#include <libpanning/LoudspeakerArray.h>
#include <libpanning/VBAP.h>
#include <libpanning/XYZ.h>

#include <libpml/double_buffering_protocol.hpp>
#include <libpml/listener_position.hpp>
#include <libpml/matrix_parameter.hpp>
#include <libpml/object_vector.hpp>
#include <libpml/shared_data_protocol.hpp>

#include <vector>

namespace visr
{
// forward declarations
namespace objectmodel
{
class ObjectVector;
}
namespace efl
{
template< typename SampleType > class BasicMatrix;
}

namespace rcl
{

/**
 * Audio component for calculating the loudspeaker gains for Higher-Order-Ambisonics (HOA) objects.
 * Implementation based on AllRAD, i.e., the HOA signals are decoded to a virtual regular loudspeaker 
 * array, whose loudspeaker signals are panned to the real, physical loudspeaker setup using VBAP.
 */
class VISR_RCL_LIBRARY_SYMBOL HoaAllRadGainCalculator: public AtomicComponent
{
public:
  /**
   * Type of the gain coefficients. We use the standard type for audio samples (default: float)
   */
  using CoefficientType = SampleType;

  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component.
   * @param numberOfObjectChannels The totl number of object signal channels. Basically this defines the size of the gain matrices processed.
   * @param regularArrayConfig The array configuration object for the virtual, regular array used for decoding the HOA signals.
   * @param realArrayConfig The array configuration object for the real, physical array to which the soundfield is panned.
   * @param decodeMatrix Matrix coefficients for decoding the HOA signals to the regular, virtual array. Dimension (hoaOrder+1)^2 x number of regular loudspeakers.
   * The row dimension determines the (maximum) HOA order and must be a square number.
   * @param listenerPosition The initial listener position used for VBAP panning. Optional argument, default is (0,0,0).
   * @param adaptiveListenerPosition Whether the rendering supports adaptation to a tracked listener In this case a parameter input port "listenerInput"
   */
  explicit HoaAllRadGainCalculator( SignalFlowContext const & context,
                                    char const * name,
                                    CompositeComponent * parent,
                                    std::size_t numberOfObjectChannels,
                                    panning::LoudspeakerArray const & regularArrayConfig,
                                    panning::LoudspeakerArray const & realArrayConfig,
                                    efl::BasicMatrix<Afloat> const & decodeMatrix,
                                    pml::ListenerPosition const & listenerPosition = pml::ListenerPosition(),
                                    bool adaptiveListenerPosition = false );

  /**
   * Disabled (deleted) copy constructor
   */
  HoaAllRadGainCalculator( HoaAllRadGainCalculator const & ) = delete;


  /**
   * Destructor.
   */
  ~HoaAllRadGainCalculator();

  /**
   * Process function implementing the runtime behaviour.
   */
  void process() override;

private:

  /**
   * Set the reference listener position. This overload accepts three Cartesian coordinates.
   * The listener positions are used beginning with the next process() call.
   * This method triggers the recalculation of the internal data (e.g., the inverse panning matrices).
   * @param x The x coordinate [in meters]
   * @param y The y coordinate [in meters]
   * @param z The z coordinate [in meters]
   * @throw std::runtime_error If the recalculation of the internal panning data fails.
   * @todo Consider extending the interface to multiple listener positions.
   */
  void setListenerPosition( CoefficientType x, CoefficientType y, CoefficientType z );

  /**
  * Set the reference listener position given as a ListenerPosition object.
  * The listener positions are used beginning with the next process() call.
  * This method triggers the recalculation of the internal data (e.g., the inverse panning matrices).
  * @param pos The listener position.
  * @throw std::runtime_error If the recalculation of the internal panning data fails.
  * @todo Consider extending the interface to multiple listener positions.
  */
  void setListenerPosition( pml::ListenerPosition const & pos );

  ParameterInput<pml::DoubleBufferingProtocol, pml::ObjectVector> mObjectInput;

  ParameterInput<pml::SharedDataProtocol, pml::MatrixParameter<SampleType> > mGainMatrixInput;

  ParameterOutput<pml::SharedDataProtocol, pml::MatrixParameter<SampleType> > mGainMatrixOutput;

  std::unique_ptr<ParameterInput<pml::DoubleBufferingProtocol, pml::ListenerPosition > > mListenerInput;

  std::unique_ptr<panning::AllRAD> mAllRadCalculator;

  /**
   * Decoding matrix from HOA signal components to the loudspeakers of the (virtual) regular array.
   * Dimension: # HOA signal components * # loudspeakers in the virtual array.
   */
  efl::BasicMatrix<Afloat> mRegularDecodeMatrix;

  efl::BasicMatrix<Afloat> mRealDecodeMatrix;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_HOA_ALLRAP_GAIN_CALCULATOR_HPP_INCLUDED
