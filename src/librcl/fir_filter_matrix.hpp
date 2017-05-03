/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_FIR_FILTER_MATRIX_HPP_INCLUDED
#define VISR_LIBRCL_FIR_FILTER_MATRIX_HPP_INCLUDED

#include <libril/atomic_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/constants.hpp>

#include <libefl/basic_matrix.hpp>
#include <libefl/basic_vector.hpp>

#include <libpml/filter_routing_parameter.hpp>

#include <cstddef> // for std::size_t
#include <memory>
#include <valarray>

namespace visr
{
// forward declaration
namespace rbbl
{
template< typename SampleType >
class MultichannelConvolverUniform;
}
  
namespace rcl
{

/**
 * A multiple input/multiple output FIR filter
 * This class has one input port named "in" and one output port named "out".
 * The widths of the input and the output port are set by the parameters \p 
 * numberOfInputs and \p numberOfOutputs in the setup() method.
 */
class FirFilterMatrix: public AtomicComponent
{
  using SampleType = visr::SampleType;
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit FirFilterMatrix( SignalFlowContext const & context,
                            char const * name,
                            CompositeComponent * parent = nullptr );

  /**
   * Desctructor
   */
  ~FirFilterMatrix();

  /**
   * Setup method to initialise the object and set the parameters.
   * @param numberOfInputs The number of input signals.
   * the multichannel input and output waveforms. .
   * @param numberOfOutputs The number of output signals.
   * @param filterLength The maximum admissible FIR filter length.
   * @param maxFilters The maximum number of filter specifications (single FIR filters) that can be stored within
   * the component.
   * @param maxRoutings The maximum number of routings (i.e., the maximum number of simultaneously convolved 
   * signal/filter combinations supported by this filter.
   * @param filters The initial FIR filters stored as matrix rows. The filters 0...filters.numberOfRows() are initialised
   * to these FIR filters, the remaining filter specifications are set to zero. Default: empty matrix, i.e., 
   * all filters are zero-initialised.
   * @param routings Initial set of filter routings. Default value: empty routing list, i.e., no signal routings are 
   * active initially.
   * @param fftImplementation name of the FFt library to be used. See rbbl::FftWrapperFactory for available names. 
   * Optional parameter, default is "default", i.e., the default FFt library for the platform.
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

  /**
   * Add a routing point in the filter matrix.
   * If there is already an entry for the combination (\p inputIdx, \p outputIdx ), the existing rntry is replaced.
   * @param inputIdx The input signal index for the routing.
   * @param outputIdx The output signal index for this routing.
   * @param filterIdx The filter index for this routing (pointing to an entry to the filter container of this component)
   * @param gain An optional, frequency-independent gain for this routing. Optional argument, defaults to 1.0.
   * @throw std::invalid_argument If \p inputIdx, \p outputIdx, or \p filterIdx exceed their respective admissible ranges
   */
  void addRouting( std::size_t inputIdx, std::size_t outputIdx, std::size_t filterIdx,
                   SampleType const gain = static_cast<SampleType>(1.0) );

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
  AudioInput mInput;

  /**
   * The audio output port for this component.
   */
  AudioOutput mOutput;

  /**
   * Vectors to the channel pointers of the input and output ports.
   * They are required by the interface of the contained rbbl::MultiChannelConvolverUniform class.
   * @todo Consider (optional) stride-based interface for rbbl::MultiChannelConvolverUniform.
   */
  //@{
  std::valarray<SampleType const *> mInputChannels;
  std::valarray<SampleType * > mOutputChannels;
  //@}

  std::unique_ptr<rbbl::MultichannelConvolverUniform<SampleType> > mConvolver;
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_FIR_FILTER_MATRIX_HPP_INCLUDED
