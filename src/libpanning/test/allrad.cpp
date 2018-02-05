//
//  main.cpp
//
//  Created by Dylan Menzies on 10/11/2014.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//

#include <libpanning/VBAP.h>
#include <libpanning/AllRAD.h>

#include <libpml/matrix_parameter.hpp>

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>


BOOST_AUTO_TEST_CASE( AllRadInstantiation )
{
  using namespace visr;
  using namespace visr::panning;

  LoudspeakerArray array, regularArray;
  // AllRAD allrad;

  // Initialisation:
  boost::filesystem::path const configDir( CMAKE_SOURCE_DIR "/config" );
  boost::filesystem::path const sourceDir( CMAKE_CURRENT_SOURCE_DIR );
  boost::filesystem::path const realArrayPath = configDir / boost::filesystem::path( "generic/octahedron.xml" );
  BOOST_CHECK( exists(realArrayPath) );
  BOOST_CHECK_NO_THROW( array.loadXmlFile(realArrayPath.string().c_str() ) );
  VBAP vbap( array );

  // file = fopen("arrays/t-design_t8_P40.txt","r");
  boost::filesystem::path const regularArrayPath = sourceDir / boost::filesystem::path( "matlab/arrays/t-design_t8_P40.xml" );
  BOOST_CHECK( exists( regularArrayPath ) );
  BOOST_CHECK_NO_THROW( regularArray.loadXmlFile( regularArrayPath.string().c_str() ) );
  //allrad.setRegArray( &regularArray );

  boost::filesystem::path const coeffFilePath = sourceDir / boost::filesystem::path( "matlab/arrays/decode_N8_P40_t-design_t8_P40.txt" );
  BOOST_CHECK( exists( coeffFilePath ) );

  pml::MatrixParameter<Afloat> coeffMtx = pml::MatrixParameter<Afloat>::fromTextFile( coeffFilePath.string() );

  //explicit AllRAD( LoudspeakerArray const & regularArray,
  //  LoudspeakerArray const & realArray,
  //  efl::BasicMatrix<Afloat> const & decodeCoeffs,
  //  unsigned int maxHoaOrder );

  std::size_t const hoaOrder = 8;

  AllRAD allRAD( regularArray, array, coeffMtx, hoaOrder );

  // Initially and every time listener moves:
  vbap.setListenerPosition( 0.0, 0.0, 0.0 );

  // Load vbap with other sources and find loudspeaker gains
  // ....
  // ....



  // Load vbap with reg-array-speaker-sources
  // Calc vbap gains then calc AllRAD b-format decode gains

  //allrad.calcDecodeGains( &vbap );
  //efl::BasicMatrix<Afloat> const & decodeGains = allrad.getDecodeGains();

  //// Write b-format2vbap gains for matlab testing:

  //// file = fopen("testoutput/decodeB2VBAP.txt","w");
  //boost::filesystem::path const sourceDir( CMAKE_CURRENT_SOURCE_DIR );
  //boost::filesystem::path const referenceCoeffPath = sourceDir / boost::filesystem::path( "referencedata/decodeB2VBAP.txt" );
  //BOOST_CHECK( exists( referenceCoeffPath ) );
  //pml::MatrixParameter<Afloat> referenceCoeffs = pml::MatrixParameter<Afloat>::fromTextFile( referenceCoeffPath.string(), 0/*alignment*/ );
  //BOOST_CHECK( referenceCoeffs.numberOfRows() == 9 ); // Reference data goes only up to 9 (second-order Ambisonics)
  //BOOST_CHECK( referenceCoeffs.numberOfColumns() == array.getNumRegularSpeakers() );

  //for( int k = 0; k < 9; k++ )
  //{ // 9 harms - 2nd order only
  //  for( std::size_t j = 0; j < array.getNumRegularSpeakers(); j++ )
  //  {
  //    BOOST_CHECK_CLOSE( decodeGains(k,j), referenceCoeffs(k,j), 0.1f ); // Allowed tolerance in percent.
  //  }
  //}
}
