//
//  main.cpp
//
//  Created by Dylan Menzies on 10/11/2014.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//

#include <libpanning/VBAP.h>
#include <libpanning/AllRAD.h>
#include <boost/filesystem.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

// Text format will be removed soon.
#if 0
BOOST_AUTO_TEST_CASE( LoudspeakerArrayLoadTextFormat )
{
  using namespace visr;
  using namespace visr::panning;

  LoudspeakerArray array, regArray;
  VBAP vbap;
  AllRAD allrad;

  std::size_t numberOfSources = 8;

  std::vector<XYZ> sourcePos( numberOfSources );

  FILE* file;
    
  boost::filesystem::path const configDir( CMAKE_SOURCE_DIR "/config" );
  boost::filesystem::path const sourceDir( CMAKE_CURRENT_SOURCE_DIR );

  // Useage / test VBAP with 8 sources around an octahedron array

//  boost::filesystem::path bfile = configDir / boost::filesystem::path("isvr/cube_audiolab.txt");
//  boost::filesystem::path bfile = configDir / boost::filesystem::path("generic/octahedron.txt");
//  boost::filesystem::path bfile = configDir / boost::filesystem::path("isvr/9.1_audiolab.txt");


  boost::filesystem::path bfile = configDir / boost::filesystem::path( "isvr/22.1_audiolab.txt" );

  BOOST_CHECK_MESSAGE( exists(bfile), "Loudspeaker configuration text file dows not exist." );

  file = fopen( bfile.string().c_str(), "r" );
  BOOST_CHECK( file != 0 );
  BOOST_CHECK( array.load( file ) != -1 );

  fclose( file );
  file = 0;
}
#endif

BOOST_AUTO_TEST_CASE( LoadArrayConfigXmlFile )
{
  using namespace visr;
  using namespace visr::panning;
  LoudspeakerArray array;

  boost::filesystem::path const configDir( CMAKE_SOURCE_DIR "/config" );
  boost::filesystem::path const sourceDir( CMAKE_CURRENT_SOURCE_DIR );

  // Alternatively, load the config file in XML format.
  boost::filesystem::path configFileXml = configDir / boost::filesystem::path( "isvr/audiolab_22speakers_1subwoofer.xml" );
  BOOST_ASSERT( exists( configFileXml ) );
  BOOST_CHECK_NO_THROW( array.loadXmlFile( configFileXml.string() ) );

  VBAP vbap;
  BOOST_CHECK_NO_THROW( vbap.setLoudspeakerArray( &array ) );
}

#if 0
{
  vbap.setListenerPosition( 0.0f, 0.0f, 0.0f );
  //    vbap.setListenerPosition(-1.9f, 0.0f, -0.1f);
  //    vbap.setListenerPosition(1.5f, 0.0f, 0.4f);
  //    vbap.setListenerPosition(-1.9f, 0.0f, +0.05f);
  //    vbap.setListenerPosition(0.0f, -0.6f, -0.2f);
  //    vbap.setListenerPosition(0.0f, -0.6f, -0.2f);  // hit triplet 39 with pw(-1,0,0)
  //    vbap.setListenerPosition(2.0f, 0.0f, 0.0f);  // hit triplet 35 with pw(0,0,-1)
  vbap.calcInvMatrices();

  sourcePos[0].set( 2.08f, 1.0f, -5.0f, true );
  //    sourcePos[0].set(-1.0f,	0.0, 0.0f, true);     // plane wave from front/back
  //    sourcePos[0].set(0.0f, -1.0f, 0.0f, true);     // plane wave from left/right
  //    sourcePos[0].set(0.0f, 0.0f, -1.0f, true);     // plane wave from below
  //    sourcePos[0].set(1.78f, 1.73f, -0.86f, false);

  //    vbap.setNumSources( numberOfSources );
  //    sourcePos[0].set(1.0, 1.0, 1.0, false);
  //    sourcePos[0].set(1.0,	0.3, -0.2, false); // for 9.1_audiolab.txt jumps between triplet 11 and 10 as z reduced
  //    sourcePos[0].set(0.0, 1.0, -0.9, false);
  sourcePos[1].set( 1.0f, 0.0f, 0.0f, false );
  sourcePos[2].set( 0.0f, 1.0f, 0.0f, false );
  sourcePos[3].set( 0.0f, 0.0f, 1.0f, false );
  sourcePos[4].set( -1.0f, 0.0f, 0.0f, false );
  sourcePos[5].set( 0.0f, -1.0f, 0.0f, false );
  sourcePos[6].set( 0.0f, 0.0f, -1.0f, false );
  sourcePos[7].set( -1.0f, -1.0f, -1.0f, false );

  vbap.setNumSources( 1 ); // This means that the output is computed only for the zeroeth source in the vector.
  vbap.setSourcePositions( &sourcePos[0] );

  vbap.calcGains();

  efl::BasicMatrix<Afloat> const & vbapGains = vbap.getGains();   // Check in watch window
  // Print the matrix.
  std::size_t const numCols = vbapGains.numberOfColumns();
  for( std::size_t rowIdx(0); rowIdx < vbapGains.numberOfRows(); ++rowIdx )
  {
    Afloat const * rowPtr = vbapGains.row( rowIdx );
    std::cout << "VBAP gain for source #" << rowIdx << ": ";
    std::copy( rowPtr, rowPtr+numCols, std::ostream_iterator<Afloat>( std::cout, ", " ) );
    std::cout << std::endl;
  }  

  // 5.1 test, 2D VBAP

  // file = fopen("arrays/5.1.txt","r");
  bfile = configDir / boost::filesystem::path( "isvr/stereo_audiolab.txt" );
  //bfile = configDir / boost::filesystem::path("isvr/5.1_audiolab.txt");
  file = fopen( bfile.string().c_str(), "r" );
  if( array.load( file ) == -1 ) return -1;
  fclose( file );
  file = 0;

  vbap.setLoudspeakerArray( &array );
  vbap.setListenerPosition( 0.0, 0.0, 0.0 );
  vbap.calcInvMatrices();

  vbap.setNumSources( 4 );
  sourcePos[0].set( 1.0, 0.0, 0.0, false );
  //    sourcePos[0].set(1.0, 0.0, 0.0, true);  // plane wave
  sourcePos[1].set( 0.0, 1.0, 0.0, false );
  sourcePos[2].set( -1.0, 0.0, 0.0, false );
  sourcePos[3].set( 0.0, -1.0, 0.0, false );
  //    sourcePos[0].set(1.0, 0.0, -5.0, false);
  //    sourcePos[1].set(0.0, 1.0, -5.0, false);
  //    sourcePos[2].set(-1.0, 0.0, -5.0, false);
  //    sourcePos[3].set(0.0, -10.0, -50.0, false);

  vbap.setSourcePositions( &sourcePos[0] );

  vbap.calcGains();

  // Check the updated values of vbapGains in watch window



  // Useage / test AllRAD ambisonic decode

  // Initialization:

  // file = fopen("arrays/octahedron.txt","r");
  bfile = configDir / boost::filesystem::path( "generic/octahedron.txt" );
  file = fopen( bfile.string().c_str(), "r" );
  if( array.load( file ) == -1 ) return -1;
  fclose( file );
  vbap.setLoudspeakerArray( &array );

  // file = fopen("arrays/t-design_t8_P40.txt","r");
  bfile = configDir / boost::filesystem::path( "generic/t-design_t8_P40.txt" );
  file = fopen( bfile.string().c_str(), "r" );
  if( regArray.load( file ) == -1 ) return -1;
  allrad.setRegArray( &regArray );
  fclose( file );

  // file = fopen("arrays/decode_N8_P40_t-design_t8_P40.txt","r");
  bfile = configDir / boost::filesystem::path( "generic/decode_N8_P40_t-design_t8_P40.txt" );
  file = fopen( bfile.string().c_str(), "r" );
  if( allrad.loadRegDecodeGains( file, 8, 40 ) == -1 ) return -1;
  fclose( file );


  // Initially and every time listener moves:

  vbap.setListenerPosition( 0.0, 0.0, 0.0 );
  vbap.calcInvMatrices();

  // Load vbap with other sources and find loudspeaker gains
  // ....
  // ....



  // Load vbap with reg-array-speaker-sources
  // Calc vbap gains then calc AllRAD b-format decode gains

  allrad.calcDecodeGains( &vbap );
  efl::BasicMatrix<Afloat> const & decodeGains = allrad.getDecodeGains();

  // Write b-format2vbap gains for matlab testing:

  // file = fopen("testoutput/decodeB2VBAP.txt","w");
  bfile = sourceDir / boost::filesystem::path( "testoutput/decodeB2VBAP.txt" );
  file = fopen( bfile.string().c_str(), "w" );
  if( file )
  {
    for( int k = 0; k < 9; k++ )
    { // 9 harms - 2nd order only
      for( std::size_t j = 0; j < vbap.getNumSpeakers(); j++ )
      {
        fprintf( file, "%f ", decodeGains(k, j) );
      }
      fprintf( file, "\n" );
    }
    fclose( file );
  }
  return 0;
}
#endif
