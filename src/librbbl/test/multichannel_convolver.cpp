/* Copyright Institue of Sound and Vibration Research - All rights reserved. */

#include <librbbl/multichannel_convolver_uniform.hpp>

#include <libefl/basic_matrix.hpp>
#include <libril/constants.hpp>
#include <libpml/matrix_parameter.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <ciso646>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace rbbl
{
namespace test
{

BOOST_AUTO_TEST_CASE( MultichannelConvolver )
{
  static const std::size_t alignment = 8; // element
  using SampleType = float;
  using Conv = MultichannelConvolverUniform<SampleType>;
  boost::filesystem::path const baseDir = CMAKE_CURRENT_SOURCE_DIR;
  pml::MatrixParameter<SampleType> const filters = pml::MatrixParameter<SampleType>::fromAudioFile( (baseDir/boost::filesystem::path("signals/filters_4ch.wav")).string(), alignment );
  pml::MatrixParameter<SampleType> const inputSignal = pml::MatrixParameter<SampleType>::fromAudioFile( (baseDir / boost::filesystem::path( "signals/input_4ch.wav" )).string( ), alignment );
  pml::MatrixParameter<SampleType> const outputSignalReference = pml::MatrixParameter<SampleType>::fromAudioFile( (baseDir / boost::filesystem::path( "signals/output_4ch.wav" )).string( ), alignment );

  std::size_t const cNumberOfInputs = inputSignal.numberOfRows();
  std::size_t const cNumberOfOutputs = outputSignalReference.numberOfRows( );
  std::size_t const cNumFilters = filters.numberOfRows();
  std::size_t const cSignalLength = inputSignal.numberOfColumns();

  std::size_t const cFilterLength = filters.numberOfColumns();

  std::size_t const cBlockLength = 32;
  BOOST_CHECK( cBlockLength % alignment == 0 );

  std::size_t const cMaxRoutings = 12;
  std::vector<Conv::RoutingEntry> routings = { { 0, 0, 0, 1.0f }, { 1, 1, 1, 1.0f } };

  efl::BasicMatrix<SampleType> outputSignal( cNumberOfOutputs, cSignalLength, alignment );

  Conv convolver( cNumberOfInputs, cNumberOfOutputs, cBlockLength, cFilterLength, cMaxRoutings,
    cNumFilters, routings, filters, alignment, "kissfft" );

  std::vector<SampleType const *> inputSignalPtr( cNumberOfInputs );
  std::vector<SampleType *> outputSignalPtr( cNumberOfOutputs );

  BOOST_CHECK_MESSAGE( cSignalLength % cBlockLength == 0 , "The signal length must be an integral multiple of the block size." );
  std::size_t const cNumBlocks = cSignalLength / cBlockLength;
  for( std::size_t blockIdx( 0 ); blockIdx < cNumBlocks; ++blockIdx )
  {
    const std::size_t signalIdx = blockIdx * cBlockLength;
    for( std::size_t inIdx( 0 ); inIdx < cNumberOfInputs; ++inIdx )
    {
      inputSignalPtr[inIdx] = inputSignal.row( inIdx ) + signalIdx;
    }
    for( std::size_t outIdx( 0 ); outIdx < cNumberOfOutputs; ++outIdx )
    {
      outputSignalPtr[outIdx] = outputSignal.row( outIdx ) + signalIdx;
    }
    convolver.process( &inputSignalPtr[0], &outputSignalPtr[0] );
  }

  for( std::size_t chIdx( 0 ); chIdx < cNumberOfOutputs; ++chIdx )
  {
    SampleType const * outSig = outputSignal.row( chIdx );
    SampleType const * outSigRef = outputSignalReference.row( chIdx );

    efl::BasicVector<SampleType> diff( cSignalLength, alignment );
    std::transform( outSig, outSig + cSignalLength, outSigRef, diff.data(), []( SampleType lhs, SampleType rhs ) { return std::abs( lhs - rhs ); } );

    SampleType * res = diff.data();

    SampleType const * maxErr = std::max_element( res, res + diff.size() );
    std::cout << "Max. error for channel " << chIdx << ": " << *res << std::endl;
    BOOST_CHECK_MESSAGE( *maxErr <= static_cast<SampleType>(4.0) * std::numeric_limits<SampleType>::epsilon(), "Convolution error exceeds 4*epsilon() for this datatype." );
  }
}

} // namespace test
} // namespace rbbl
} // namespace visr
