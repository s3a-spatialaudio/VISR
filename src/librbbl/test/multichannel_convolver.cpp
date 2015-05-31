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

BOOST_AUTO_TEST_CASE( readFiltersFromWavFile )
{
  using SampleType = float;
  std::size_t const alignElements = 8;

  boost::filesystem::path const baseDir( CMAKE_CURRENT_SOURCE_DIR );
  boost::filesystem::path const impulseWav = baseDir / boost::filesystem::path( "fir/quasiAllpassFIR_f32_n63.wav" );
  BOOST_CHECK( exists( impulseWav ) and not is_directory( impulseWav ) );

  pml::MatrixParameter<SampleType> const impulses
    = pml::MatrixParameter<SampleType>::fromAudioFile( impulseWav.string( ), alignElements );

  std::size_t irChannels = impulses.numberOfRows();

#if 0
  std::size_t irLength = impulses.numberOfColumns();
  for( std::size_t rowIdx( 0 ); rowIdx < irChannels; ++rowIdx )
  {
    std::cout << "Filter " << rowIdx << ": ";
    std::copy( impulses.row( rowIdx ), impulses.row( rowIdx ) + irLength, std::ostream_iterator<SampleType>( std::cout, ", " ) );
    std::cout << std::endl;
  }
#endif

  std::vector< rbbl::MultichannelConvolverUniform<SampleType>::RoutingEntry > routings
    = { { 0, 0, 0, 1.0f }, { 1, 1, 1, 0.5f } };

  //explicit MultichannelConvolverUniform( std::size_t numberOfInputs,
  //  std::size_t numberOfOutputs,
  //  std::size_t blockLength,
  //  std::size_t maxFilterLength,
  //  std::size_t maxRoutingPoints,
  //  std::size_t maxFilterEntries,
  //  std::vector<RoutingEntry> const & initialRoutings,
  //  efl::BasicMatrix<SampleType> const & initialFilters,
  //  std::size_t alignment = 0 );
  rbbl::MultichannelConvolverUniform<SampleType> conv( 3, 5, 16, 128, 12, irChannels, routings, impulses, alignElements );

}

BOOST_AUTO_TEST_CASE( readFiltersFromTextFile )
{
  using SampleType = double;
  std::size_t const alignElements = 8;

  boost::filesystem::path const baseDir( CMAKE_CURRENT_SOURCE_DIR );
  boost::filesystem::path const impulseTxt = baseDir / boost::filesystem::path( "fir/quasiAllpassFIR_f32_n63.txt" );
  BOOST_CHECK( exists( impulseTxt ) and not is_directory( impulseTxt ) );

  pml::MatrixParameter<SampleType> const impulses
    = pml::MatrixParameter<SampleType>::fromTextFile( impulseTxt.string(), alignElements );

#if 0
  std::size_t irChannels = impulses.numberOfRows();
  std::size_t irLength = impulses.numberOfColumns();
  for( std::size_t rowIdx( 0 ); rowIdx < irChannels; ++rowIdx )
  {
    std::cout << "Filter " << rowIdx << ": ";
    std::copy( impulses.row( rowIdx ), impulses.row( rowIdx ) + irLength, std::ostream_iterator<SampleType>( std::cout, ", " ) );
    std::cout << std::endl;
  }
#endif
}

BOOST_AUTO_TEST_CASE( readFiltersFromStrings )
{
  using SampleType = double;
  std::size_t const alignElements = 8;

  std::string filterDef( " 0.0 0.75 -3.0\n 0.0, -1, +3.275 % comments are allowed \n \n    % Also lines with only comments" );

  pml::MatrixParameter<SampleType> const impulses
    = pml::MatrixParameter<SampleType>::fromString( filterDef, alignElements );

  std::size_t irChannels = impulses.numberOfRows( );
  std::size_t irLength = impulses.numberOfColumns( );

  for( std::size_t rowIdx( 0 ); rowIdx < irChannels; ++rowIdx )
  {
    std::cout << "Filter " << rowIdx << ": ";
    std::copy( impulses.row( rowIdx ), impulses.row( rowIdx ) + irLength, std::ostream_iterator<SampleType>( std::cout, ", " ) );
    std::cout << std::endl;
  }
}



} // namespace test
} // namespace rbbl
} // namespace visr
