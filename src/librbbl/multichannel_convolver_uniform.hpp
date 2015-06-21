/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_MULTICHANNEL_CONVOLVER_UNIFORM_HPP_INCLUDED
#define VISR_LIBRBBL_MULTICHANNEL_CONVOLVER_UNIFORM_HPP_INCLUDED

#include <libefl/basic_matrix.hpp>
#include <libefl/basic_vector.hpp>

#include <libpml/filter_routing_parameter.hpp>

#include <librbbl/circular_buffer.hpp>
#include <librbbl/fft_wrapper_base.hpp>

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
class MultichannelConvolverUniform
{
public:
  /**
   * The representation for the complex frequency-domain elements.
   * Note: The used FFT libraries depend on the fact that arrays of this type
   * can be cast into arrays of the corresponding real types with interleaved
   * real and imaginary elements.
   * SampleType arrays with interleaved real and imaginary components.
   */
  using FrequencyDomainType = typename FftWrapperBase<SampleType>::FrequencyDomainType;

  /**
   * The data type for specifying routing points.
   */
  using RoutingEntry = pml::FilterRoutingParameter;

  /**
    The data type for setting sets of routing points.
   */
  using RoutingList = pml::FilterRoutingList;

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
                                         RoutingList const & initialRoutings,
                                         efl::BasicMatrix<SampleType> const & initialFilters,
                                         std::size_t alignment = 0,
                                         char const * fftImplementation = "default" );

  /**
   * Destructor.
   */
  ~MultichannelConvolverUniform();

  std::size_t numberOfInputs() const { return mNumberOfInputs; }

  std::size_t numberOfOutputs() const { return mNumberOfOutputs; }

  std::size_t blockLength() const { return mBlockLength; }

  std::size_t maxNumberOfRoutingPoints() const { return mMaxNumberOfRoutingPoints; }

  std::size_t maxNumberOfFilterEntries() const { return mFilterPartitionsFrequencyDomain.numberOfRows(); }

  std::size_t maxFilterLength() const { return mMaxFilterLength; }

  std::size_t numberOfRoutingPoints( ) const { return mRoutingTable.size(); }

  void process( SampleType const * const * input,
                SampleType * const * output,
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
  void initRoutingTable( RoutingList const & routings );

  /**
  * Add a new routing to the routing table.
  * @throw std::invalid_argument If adding the entry would exceed the maximally permitted number of routings
  * @throw std::invalid_argument If any input, output, or filter index exceeds the admissible range for the respective type.
  */
  void setRoutingEntry( RoutingEntry const & routing );

  /**
  * Add a new routing to the routing table.
  * @throw std::invalid_argument If adding the entry would exceed the maximally permitted number of routings
  * @throw std::invalid_argument If any input, output, or filter index exceeds the admissible range for the respective type.
  */
  void setRoutingEntry( std::size_t inputIdx, std::size_t outputIdx, std::size_t filterIdx, RoutingEntry::GainType gain );

  /**
  * @return \p true if the entry was removes, \p false if not (i.e., the entry did not exist).
  */
  bool removeRoutingEntry( std::size_t inputIdx, std::size_t outputIdx );

  /**
  * Query the currently active number of routings.
  */
  std::size_t numberOfRoutings( ) const { return mRoutingTable.size( ); }
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

  void transformImpulseResponse( SampleType const * ir, std::size_t irLength, FrequencyDomainType * result, std::size_t alignment = 0 );

  void setImpulseResponse( SampleType const * ir, std::size_t filterLength, std::size_t filterIdx, std::size_t alignment = 0 );

  void processInput( SampleType const * const * input, std::size_t alignment );

  void processOutput( SampleType * const * output, std::size_t alignment );

  /**
   * Helper functions to calculate the parameters of the partitioned convolution algorithm.
   */
  //@{
  /**
   * Calculate the number of filter partitions for the nonuniformly partitioned convolution algorithm.
   */
  static std::size_t calculateNumberOfPartitions( std::size_t filterLength, std::size_t blockLength );

  /** 
   * Return the size of the DFT, i.e., the number of real inputs samples passed to each forward FFT call.
   */
  static std::size_t calculateDftSize( std::size_t blockLength );

  /**
   * Return the number of complex values to represent the frequency-domain DFT representation.
   */
  static std::size_t calculateDftRepresentationSize( std::size_t blockLength );

  /**
  * Return the number of complex values to represent the frequency-domain DFT representation when padded to the requested alignment.
  * @param blockLength The number of values consumed and produced in each process() call.
  * @param alignment The alignment (in number of complex elements)
  */
  static std::size_t calculateDftRepresentationSizePadded( std::size_t blockLength, std::size_t alignment );

  /**
   * Calculate the scaling factor to be applied to the transformed input filters.
   * This factor is necessary to account for the different normalisation strategies used in different FFT libraries 
   * (i.e., whether normalization is done in the forward or the inverse transform, distributed between the two (unitary transform), or not at all.
   * @throw std::logic_error If the Fft representation object has not been initialised.
   */
  SampleType calculateFilterScalingFactor() const;
  //@}

  inline FrequencyDomainType * getFdlBlock( std::size_t inputIdx, std::size_t blockIdx );

  inline FrequencyDomainType const * getFdlBlock( std::size_t inputIdx, std::size_t blockIdx ) const;

  inline FrequencyDomainType * getFdFilterPartition( std::size_t filterIdx, std::size_t blockIdx );

  inline FrequencyDomainType const * getFdFilterPartition( std::size_t filterIdx, std::size_t blockIdx ) const;

  /**
   * Assign an already transformed filter to a specific index of the the filter matrix.
   * @param transformedFilter The frequency-domain representation of the filter.
   * @param filterIdx The filter index to which the filter is assigned.
   * @param alignment The guaranteed alignment of the transformedFilter argument (as a multiple of the size of the complex data type FrequencyDomainType.
   */
  void setFilter( FrequencyDomainType const * transformedFilter, std::size_t filterIdx, std::size_t alignment = 0 );
  //@}

  /**
   * The alignment used in all data members.
   * Also used for the representation of the FDL and the frequency-domain filter representation, which stores all partitions in a single matrix row, 
   * possibly using zero padding to enforce the required alignment for each partition.
   */
  std::size_t const mAlignment;

  std::size_t const mComplexAlignment;

  std::size_t const mNumberOfInputs;

  std::size_t const mNumberOfOutputs;

  std::size_t const mBlockLength;

  std::size_t const mMaxNumberOfRoutingPoints;

  std::size_t const mMaxFilterLength;

  std::size_t const mNumberOfFilterPartitions;

  /**
   * The size of a single DFT transform.
   */
  std::size_t const mDftSize;

  std::size_t const mDftRepresentationSize;

  std::size_t const mDftRepresentationSizePadded;

  struct RoutingKey
  {
    explicit RoutingKey( std::size_t in, std::size_t out )
    : inputIdx( in ), outputIdx( out )
    {
    }
    std::size_t inputIdx;
    std::size_t outputIdx;
  };
  /**
   * Function object for ordering the routings within the routing table.
   * By grouping the entries according to the output indices, the ordering is 
   * specifically tailored to the execution of the process() function.
   */
  struct CompareRoutings
  {
    bool operator()( RoutingKey const & lhs, RoutingKey const & rhs ) const
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

  struct RoutingValue
  {
    RoutingValue( std::size_t idx, SampleType gain )
    : filterIdx( idx ), gainLinear( gain ) {}
    std::size_t filterIdx;
    SampleType gainLinear;
  };
  using RoutingTable = std::map<RoutingKey, RoutingValue, CompareRoutings>;

  RoutingTable mRoutingTable;

  rbbl::CircularBuffer<SampleType> mInputBuffers;

  efl::BasicMatrix<std::complex<SampleType> > mInputFDL;
  
  /**
   * Cyclic counter denoting the current block offset of the zeroth 
   * frequency-domain input block within the rows of \p mInputFDL.
   */
  std::size_t mFdlCycleOffset;
  
  /**
   * Temporary memory buffer for transforming filter partitions and holding the results of
   * the inverse transformation.
   */
  mutable efl::BasicVector<SampleType> mTimeDomainTransformBuffer;


  efl::BasicMatrix<std::complex<SampleType> > mFilterPartitionsFrequencyDomain;

  /**
   * Temporarily used buffer to accumulate the results of a frequency-domain block convolution.
   */
  efl::BasicVector<std::complex<SampleType> > mFrequencyDomainAccumulator;

  /**
   * Temporarily used buffer to combine all contributions to an output.
   */
  efl::BasicVector<std::complex<SampleType> > mFrequencyDomainSum;

  std::unique_ptr<rbbl::FftWrapperBase<SampleType> > mFftRepresentation;

  /**
   * Scaling factor applied to the frequency-domain representation of the filters to compensate
   * for the normalisation strategy of the FFT implementation in use.
   */
  SampleType const mFilterScalingFactor;
};

template<typename SampleType>
/*inline*/ typename MultichannelConvolverUniform<SampleType>::FrequencyDomainType * 
MultichannelConvolverUniform<SampleType>::getFdlBlock( std::size_t inputIdx, std::size_t blockIdx )
{
  assert( inputIdx < mNumberOfInputs );
  assert( blockIdx < mNumberOfFilterPartitions );
  std::size_t const columnIdx = ((mFdlCycleOffset + blockIdx) % mNumberOfFilterPartitions) * mDftRepresentationSizePadded;
  return mInputFDL.row( inputIdx ) + columnIdx;
}

template<typename SampleType>
/*inline*/ typename MultichannelConvolverUniform<SampleType>::FrequencyDomainType const * 
MultichannelConvolverUniform<SampleType>::getFdlBlock( std::size_t inputIdx, std::size_t blockIdx ) const
{
  assert( inputIdx < mNumberOfInputs );
  assert( blockIdx < mNumberOfFilterPartitions );
  std::size_t const columnIdx = ((mFdlCycleOffset + blockIdx) % mNumberOfFilterPartitions) * mDftRepresentationSizePadded;
  return mInputFDL.row( inputIdx) + columnIdx;
}

template<typename SampleType>
/*inline*/ typename MultichannelConvolverUniform<SampleType>::FrequencyDomainType * 
MultichannelConvolverUniform<SampleType>::
getFdFilterPartition( std::size_t filterIdx, std::size_t blockIdx )
{
  assert( filterIdx < maxNumberOfFilterEntries() );
  assert( blockIdx < mNumberOfFilterPartitions );
  std::size_t const columnIdx = blockIdx * mDftRepresentationSizePadded;
  return mFilterPartitionsFrequencyDomain.row( filterIdx ) + columnIdx;
}

template<typename SampleType>
/*inline*/ typename MultichannelConvolverUniform<SampleType>::FrequencyDomainType const * 
MultichannelConvolverUniform<SampleType>::getFdFilterPartition( std::size_t filterIdx, std::size_t blockIdx ) const
{
  assert( filterIdx < maxNumberOfFilterEntries( ) );
  assert( blockIdx < mNumberOfFilterPartitions );
  std::size_t const columnIdx = blockIdx * mDftRepresentationSizePadded;
  return mFilterPartitionsFrequencyDomain.row( filterIdx ) + columnIdx;
}

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_MULTICHANNEL_CONVOLVER_UNIFORM_HPP_INCLUDED
