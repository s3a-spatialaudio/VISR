/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_MULTICHANNEL_CONVOLVER_UNIFORM_HPP_INCLUDED
#define VISR_LIBRBBL_MULTICHANNEL_CONVOLVER_UNIFORM_HPP_INCLUDED

#include "core_convolver_uniform.hpp"
#include "export_symbols.hpp"
#include "filter_routing.hpp"

#include <libefl/basic_matrix.hpp>

#include <cassert>
#include <complex>
#include <initializer_list>
#include <map>
#include <memory>
#include <vector>

namespace visr
{
namespace rbbl
{

/**
 * Generic class for MIMO convolution using a uniformly partioned fast convolution algorithm.
 * It supports a arbitrary numbers of input and output channels and enables filters and an
 * optional gain factor for arbitrary input-output connections.
 * @tparam SampleType The floating-point type of the signal samples
 */
template< typename SampleType >
class VISR_RBBL_LIBRARY_SYMBOL MultichannelConvolverUniform
{
public:
  /**
   * Constructor.
   * @param numberOfInputs The number of input signals processed.
   * @param numberOfOutputs The number of output channels produced.
   * @param blockLength The numbers of samples processed for each input or output channels in one process() call.
   * @param maxFilterLength The maximum length of the FIR filters (in samples).
   * @param maxRoutingPoints The maximum number of routing points between input and output channels, i.e., the number of filter operations to be calculated.
   * @param maxFilterEntries The maximum number of filters that can be stored within the filter. This number can be different from \p maxRoutingPoints, as the convolver can 
   * both reuse the same filter representation multiple times, or store multiple filter representations to enable switching of the filter characteristics at runtime.
   * @param initialRoutings The initial set of routing points.
   * @param initialFilters The initial set of filter coefficients. The matrix rows represent the distinct filters.
   * @param alignment The alignment (given as a multiple of the sample type size) to be used to allocate all data structure. It also guaranteees 
   * the alignment of the input and output samples to the process call. 
   * @param fftImplementation A string to determine the FFT wrapper to be used. The default value results in using the default FFT implementation for the given data type.
   */
  explicit MultichannelConvolverUniform( std::size_t numberOfInputs,
                                         std::size_t numberOfOutputs,
                                         std::size_t blockLength,
                                         std::size_t maxFilterLength,
                                         std::size_t maxRoutingPoints,
                                         std::size_t maxFilterEntries,
                                         FilterRoutingList const & initialRoutings = FilterRoutingList(),
                                         efl::BasicMatrix<SampleType> const & initialFilters = efl::BasicMatrix<SampleType>(),
                                         std::size_t alignment = 0,
                                         char const * fftImplementation = "default" );

  /**
   * Destructor.
   */
  ~MultichannelConvolverUniform();

  std::size_t numberOfInputs() const { return mCoreConvolver.numberOfInputs(); }

  std::size_t numberOfOutputs() const { return mCoreConvolver.numberOfOutputs(); }

  std::size_t blockLength() const { return mCoreConvolver.blockLength(); }

  std::size_t maxNumberOfRoutingPoints() const { return mMaxNumberOfRoutingPoints; }

  std::size_t maxNumberOfFilterEntries() const { return mCoreConvolver.maxNumberOfFilterEntries(); }

  std::size_t maxFilterLength() const { return mCoreConvolver.maxFilterLength(); }

  std::size_t numberOfRoutingPoints( ) const { return mRoutingTable.size(); }

  void process( SampleType const * const input, std::size_t inputStride,
                SampleType * const output, std::size_t outputStride,
                std::size_t alignment = 0 );

  /**
  * Manipulation of the routing table.
  */
  //@{
  /**
  * Remove all entries from the routing table.
  */
  void clearRoutingTable( );

  /**
  * Initialize the routing table from a set of entries.
  * All pre-existing entries are cleared beforehand.
  * @param routings A vector of routing entries
  * @throw std::invalid_argument If the number of new entries exceeds the maximally permitted number of routings
  * @throw std::invalid_argument If any input, output, or filter index exceeds the admissible range for the respective type.
  */
  void initRoutingTable( FilterRoutingList const & routings );

  /**
  * Add a new routing to the routing table.
  * @throw std::invalid_argument If adding the entry would exceed the maximally permitted number of routings
  * @throw std::invalid_argument If any input, output, or filter index exceeds the admissible range for the respective type.
  */
  void setRoutingEntry( FilterRouting const & routing );

  /**
  * Add a new routing to the routing table.
  * @throw std::invalid_argument If adding the entry would exceed the maximally permitted number of routings
  * @throw std::invalid_argument If any input, output, or filter index exceeds the admissible range for the respective type.
  */
  void setRoutingEntry( std::size_t inputIdx, std::size_t outputIdx, std::size_t filterIdx, SampleType gain );

  /**
  * @return \p true if the entry was removes, \p false if not (i.e., the entry did not exist).
  */
  bool removeRoutingEntry( std::size_t inputIdx, std::size_t outputIdx );

  /**
  * Query the currently active number of routings.
  */
//  std::size_t numberOfRoutings( ) const { return mCoreConvolver.numberOfRoutings(); }
  //@}

  /**
  * Manipulation of the contained filter representation.
  */
  //@{
  /**
  * Reset all filters to zero.
  */
  void clearFilters( );

  /**
   * Load a new set of impulse responses, resetting all prior loaded filters.
   * If there are fewer filters in the matrix than the maximum admissible number, the remaining 
   * filters are set to zero. Likewise, if the length of the new filters is less than the maximum allowed filter length, 
   * the remaining elements are set to zero.
   * @param newFilters The matrix of new filters, with each row representing a filter.
   * @throw std::invalid_argument If the number of filters (number of rows) exceeds the maximum admissible number of filters.
   * @throw std::invalid_argumeent If the length of the filters (number of matrix columns) exceeds the maximum admissible length,
   */
  void initFilters( efl::BasicMatrix<SampleType> const & newFilters );

  void setImpulseResponse( SampleType const * ir, std::size_t filterLength, std::size_t filterIdx, std::size_t alignment = 0 );

private:
  /**
   * Internal function to apply the filters and to set the oputputs.
   */
  void processOutputs( SampleType * const output, std::size_t outputChannelStride,
                       std::size_t alignment );

  CoreConvolverUniform<SampleType> mCoreConvolver;

  struct RoutingEntry
  {
    explicit RoutingEntry( std::size_t in, std::size_t out, std::size_t filter, SampleType gain = 1.0f )
      : inputIdx( in ), outputIdx( out ), filterIdx( filter ), gainLinear( gain )
    {
    }
    std::size_t inputIdx;
    std::size_t outputIdx;
    std::size_t filterIdx;
    SampleType gainLinear;
  };
  /**
  * Function object for ordering the routings within the routing table.
  * By grouping the entries according to the output indices, the ordering is
  * specifically tailored to the execution of the process() function.
  */
  struct CompareRoutings
  {
    bool operator()( RoutingEntry const & lhs, RoutingEntry const & rhs ) const
    {
      if( lhs.outputIdx == rhs.outputIdx )
      {
        return lhs.inputIdx < rhs.inputIdx;
      }
      else
      {
        return lhs.outputIdx < rhs.outputIdx;
      }
    }
  };

  using RoutingTable = std::multiset<RoutingEntry, CompareRoutings>;

  RoutingTable mRoutingTable;

  std::size_t const mMaxNumberOfRoutingPoints;

  efl::BasicMatrix< typename CoreConvolverUniform<SampleType>::FrequencyDomainType > mFrequencyDomainOutput;
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_MULTICHANNEL_CONVOLVER_UNIFORM_HPP_INCLUDED
