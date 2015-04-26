/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_MULTICHANNEL_CONVOLVER_UNIFORM_HPP_INCLUDED
#define VISR_LIBRBBL_MULTICHANNEL_CONVOLVER_UNIFORM_HPP_INCLUDED

#include <libefl/basic_matrix.hpp>
#include <libefl/basic_vector.hpp>

#include <librbbl/circular_buffer.hpp>
#include <librbbl/fft_wrapper_base.hpp>


#include <cassert>
#include <complex>
#include <initializer_list>
#include <map>
#include <memory>
//#include <stdexcept>
// #include <vector>

namespace visr
{
namespace rbbl
{

/**
 * @tparam SampleType The floating-point type of the signal samples
 */
template< typename SampleType >
class MultichannelConvolverUniform
{
public:
  /**
   * The representation for the complex frequency-domain elements.
   * Note: The used FFT libraries depend on the fact that arrays of this type can be casted into 
   * SampleType arrays with interleaved real and imaginary components.
   */
  using FrequencyDomainType = typename FftWrapperBase<SampleType>::FrequencyDomainType;

  struct RoutingEntry
  {
    std::size_t output;
    std::size_t input;
    SampleType gain;
    std::size_t filterIndex;
  };

  explicit MultichannelConvolverUniform( std::size_t numberOfInputs,
                                         std::size_t numberOfOutputs,
                                         std::size_t blockLength,
                                         std::size_t maxFilterLength,
                                         std::size_t maxRoutingPoints,
                                         std::size_t maxFilterEntries,
                                         std::initializer_list<RoutingEntry> const & initialRoutings,
                                         efl::BasicMatrix<SampleType> const & initialFilters,
                                         std::size_t alignment = 0 );

  ~MultichannelConvolverUniform();

  std::size_t numberOfInputs() const { return mNumberOfInputs; }

  std::size_t numberOfOutputs() const { return mNumberOfOutputs; }

  std::size_t blockLength() const { return mBlockLength; }

  std::size_t maxNumberOfRoutingPoints() const { return mMaxNumberOfRoutingPoints; }

  std::size_t maxNumberOfFilterEntries() const { return mFilterPartitionsFrequencyDomain.numberOfRows(); }

  std::size_t numberOfRoutingPoints( ) const { return mRoutingTable.size(); }

  void process( SampleType const * const * input,
                SampleType * const * output,
                std::size_t alignment = 0 );
private:
  void processInput( SampleType const * const * input, std::size_t alignment );

  void processOutput( SampleType * const * output, std::size_t alignment );

  /**
   * Calculate the number of filter partitions for the nonuniformly partitioned convolution algorithm.
   */
  static std::size_t calculateNumberOfPartitions( std::size_t filterLength, std::size_t blockLength );

  static std::size_t calculateDftSize( std::size_t blockLength );

  static std::size_t calculateDftRepresentationSizeRealValues( std::size_t blockLength );

  static std::size_t calculateDftRepresentationSizeRealValuesPadded( std::size_t blockLength, std::size_t alignment );

  inline FrequencyDomainType * getFdlBlock( std::size_t inputIdx, std::size_t blockIdx );

  inline FrequencyDomainType const * getFdlBlock( std::size_t inputIdx, std::size_t blockIdx ) const;

  inline FrequencyDomainType * getFdFilterPartition( std::size_t filterIdx, std::size_t blockIdx );

  inline FrequencyDomainType const * getFdFilterPartition( std::size_t filterIdx, std::size_t blockIdx ) const;

  /**
   * The alignment used in all data members.
   * Also used for the representation of the FDL and the frequency-domain filter representation, which stores all partitions in a single matrix row, 
   * possibly using zero padding to enforce the required alignment for each partition.
   */
  std::size_t mAlignment;

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
    //bool operator<(RoutingKey const & rhs)
    //{
    //  return outputIdx < rhs.outputIdx ? true : inputIdx < rhs.inputIdx;
    //}
  };
  struct CompareRoutings
  {
    bool operator()( RoutingKey const & lhs, RoutingKey const & rhs ) const
    {
      return lhs.outputIdx < rhs.outputIdx ? true : lhs.inputIdx < rhs.inputIdx;
    }
  };

  struct RoutingValue
  {
    SampleType gainLinear;
    std::size_t filterIndex;
  };
  using RoutingTable = std::map<RoutingKey, RoutingValue, CompareRoutings>;

  RoutingTable mRoutingTable;

  rbbl::CircularBuffer<SampleType> mInputBuffers;

  efl::BasicMatrix<std::complex<SampleType> > mInputFDL;

  std::size_t mFdlCycleOffset;

  efl::BasicMatrix<std::complex<SampleType> > mFilterPartitionsFrequencyDomain;

  std::unique_ptr<rbbl::FftWrapperBase<SampleType> > mFftRepresentation;

  efl::BasicVector<std::complex<SampleType> > mFrequencyDomainAccumulator;
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
  std::size_t const columnIdx = (mFdlCycleOffset + blockIdx) * mDftRepresentationSizePadded;
  return mInputFDL.row( filterIdx ) + columnIdx;
}

template<typename SampleType>
/*inline*/ typename MultichannelConvolverUniform<SampleType>::FrequencyDomainType const * 
MultichannelConvolverUniform<SampleType>::getFdFilterPartition( std::size_t filterIdx, std::size_t blockIdx ) const
{
  assert( filterIdx < maxNumberOfFilterEntries( ) );
  assert( blockIdx < mNumberOfFilterPartitions );
  std::size_t const columnIdx = (mFdlCycleOffset + blockIdx) * mDftRepresentationSizePadded;
  return mInputFDL.row( filterIdx ) + columnIdx;
}

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_MULTICHANNEL_CONVOLVER_UNIFORM_HPP_INCLUDED
