/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_FIR_FILTER_MATRIX_HPP_INCLUDED
#define VISR_LIBRCL_FIR_FILTER_MATRIX_HPP_INCLUDED

#include <libril/audio_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/constants.hpp>

#include <libefl/basic_matrix.hpp>
#include <libefl/basic_vector.hpp>

#include <libpml/filter_routing_parameter.hpp>

#include <librbbl/multichannel_convolver_uniform.hpp>

#include <cstddef> // for std::size_t
#include <memory>

namespace visr
{

namespace rcl
{
// forward declaration
//namespace rbbl
//{
//template< typename SampleType >
//class MultichannelConvolverUniform;
//}

/**
 * A multichannel IIR filter consisting of a fixed, arbitrary number of biquads per channel.
 * This class has one input port named "in" and one output port named "out".
 * The widths of the input and the output port are identical and is
 * set by the argument <b>numberOfChannels</b> in the setup() method.
 */
class FirFilterMatrix: public ril::AudioComponent
{
  using SampleType = ril::SampleType;
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit FirFilterMatrix( ril::AudioSignalFlow& container, char const * name );

  /**
   * Desctructor
   */
  ~FirFilterMatrix();

  /**
   * Setup method to initialise the object and set the parameters.
   * @param numberOfInputs The number of input signals.
   * the multichannel input and output waveforms. .
   * @param numberOfOutputs The number of output signals.
   */
  void setup( std::size_t numberOfInputs,
              std::size_t numberOfOutputs,
              std::size_t filterLength,
              std::size_t maxFilters,
              std::size_t maxRoutings,
              efl::BasicMatrix<SampleType> const & filters = efl::BasicMatrix<SampleType>(),
              pml::FilterRoutingList const & routings = pml::FilterRoutingList() );

  /**
   * The process method performs the multichannel convolution.
   */
  void process( );

  /**
   * Clear all routings points.
   * The filters remain initialised.
   */
  void clearRoutings();

  void addRouting( std::size_t inputIdx, std::size_t outputIdx, std::size_t filterIdx, SampleType const gain );

  void addRouting( pml::FilterRoutingParameter const & routing );

  void addRoutings( pml::FilterRoutingList const & routings );

  void setRoutings( pml::FilterRoutingList const & routings );

  bool removeRouting( std::size_t inputIdx, std::size_t outputIdx );

  void clearFilters( );

  void setFilter( std::size_t filterIdx, SampleType const * const impulseResponse, std::size_t filterLength, std::size_t alignment =0 );

  void setFilters( efl::BasicMatrix<SampleType> const & filterSet );

private:
  /**
   * The audio input port for this component.
   */
  ril::AudioInput mInput;

  /**
   * The audio output port for this component.
   */
  ril::AudioOutput mOutput;

  std::unique_ptr<rbbl::MultichannelConvolverUniform<SampleType> > mConvolver;
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_FIR_FILTER_MATRIX_HPP_INCLUDED
