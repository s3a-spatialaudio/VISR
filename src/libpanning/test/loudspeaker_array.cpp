//
//  main.cpp
//
//  Created by Dylan Menzies on 10/11/2014.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//

#include <libpanning/VBAP.h>
#include <libpanning/AllRAD.h>
#include <boost/filesystem.hpp>
#include <libefl/cartesian_spherical_conversion.hpp>
#include <libefl/degree_radian_conversion.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>
#include <array>

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

#if 0
BOOST_AUTO_TEST_CASE( LoadArrayConfigXmlString ){
  using namespace visr;
  using namespace visr::panning;
  LoudspeakerArray array;

  //boost::filesystem::path const configDir( CMAKE_SOURCE_DIR "/config" );
  //boost::filesystem::path const sourceDir( CMAKE_CURRENT_SOURCE_DIR );

  //// Alternatively, load the config file in XML format.
  //boost::filesystem::path configFileXml = configDir / boost::filesystem::path( "isvr/audiolab_5.1_1subwoofer.xml" );
  //BOOST_ASSERT( exists( configFileXml ) );
  //BOOST_CHECK_NO_THROW( array.loadXmlFile( configFileXml.string() ) );
  //
  std::string xml = "<?xml version=\"1.0\" encoding=\"utf - 8\"?>"
  "<panningConfiguration dimension = \"2\" infinite = \"false\">"
  "<outputEqConfiguration numberOfBiquads = \"1\" type = \"iir\">"
  "<filterSpec name = \"lowpass\">"
  "<biquad a1 = \"-1.9688283\" a2 = \"0.96907117\" b0 = \"6.0729856e-05\" b1 = \"0.00012145971\" b2 = \"6.0729856e-05\"/>"
  "</filterSpec>"
  "<filterSpec name = \"highpass\">"
  "<biquad a1 = \"-1.9688283\" a2 = \"0.96907117\" b0 = \"-0.98447486\" b1 = \"1.9689497\" b2 = \"-0.98447486\"/>"
  "</filterSpec>"
  "</outputEqConfiguration>"
  "<loudspeaker channel = \"27\" delay = \"0\" eq = \"highpass\" gainDB = \"0\" id = \"1\">"
  "<cart x = \"2.17\" y = \"1.36\" z = \"0\"/>"
  "</loudspeaker>"
  "<loudspeaker channel = \"16\" delay = \"0.0011502\" eq = \"highpass\" gainDB = \"-2.1488\" id = \"2\">"
  "<cart x = \"2.08\" y = \"0.03\" z = \"0\"/>"
  "</loudspeaker>"
  "<loudspeaker channel = \"17\" delay = \"9.8764e-05\" eq = \"highpass\" gainDB = \"-0.35712\" id = \"3\">"
  "<cart x = \"2.15\" y = \"-1.22\" z = \"0.01\"/>"
  "</loudspeaker>"
  "<loudspeaker channel = \"20\" delay = \"0.0015247\" eq = \"highpass\" gainDB = \"-1.8169\" id = \"4\">"
  "<cart x = \"-1.01\" y = \"-1.69\" z = \"0\"/>"
  "</loudspeaker>"
  "<loudspeaker channel = \"24\" delay = \"0.0016965\" eq = \"highpass\" gainDB = \"-2.0675\" id = \"5\">"
  "<cart x = \"-0.98\" y = \"1.74\" z = \"0\"/>"
  "</loudspeaker>"
  "<subwoofer assignedLoudspeakers = \"1, 2, 3, 4, 5\" channel = \"41\" delay = \"0\" eq = \"lowpass\" gainDB = \"0\" weights = \"1.000000, 1.000000, 1.000000, 1.000000, 1.000000\"/>"
  "<triplet l1 = \"4\" l2 = \"5\"/>"
  "<triplet l1 = \"3\" l2 = \"4\"/>"
  "<triplet l1 = \"2\" l2 = \"3\"/>"
  "<triplet l1 = \"1\" l2 = \"2\"/>"
  "<triplet l1 = \"5\" l2 = \"1\"/>"
  "</panningConfiguration>";
  //std::cout<<xml<<std::endl;
  array.loadXmlString( xml );
  VBAP vbap( array, 0.0f, 0.0f, 0.0f );

  std::vector<SampleType> vbapGains;
  std::fill( vbapGains.begin(), vbapGains.end(), 0.0f );
  vbapGains.resize( array.getNumRegularSpeakers() );
  vbap.calculateGains( 2.08f, 1.0f, -5.0f, vbapGains.data() );
  //vbap.calculateGains( 1.0f, 1.0f, 0.0f, vbapGains.data() );
  std::size_t const numCols = array.getNumRegularSpeakers();

  std::cout << "VBAP gain for source : ";
  std::copy( vbapGains.data(), vbapGains.data() + numCols, std::ostream_iterator<Afloat>( std::cout, ", " ) );
  std::cout << std::endl;
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

    boost::filesystem::path configFileXml = configDir / boost::filesystem::path( "bbc/bs2051-4+5+0.xml" );

    //boost::filesystem::path configFileXml = configDir / boost::filesystem::path( "isvr/audiolab_22speakers_1subwoofer.xml" );
  BOOST_ASSERT( exists( configFileXml ) );
  BOOST_CHECK_NO_THROW( array.loadXmlFile( configFileXml.string() ) );
  
  
  std::vector<std::size_t> chan ( array.getLoudspeakerChannels(), array.getLoudspeakerChannels()+ array.getNumRegularSpeakers());
  std::vector<LoudspeakerArray::ChannelIndex>::iterator pos = std::adjacent_find( chan.begin(), chan.end(), std::greater<std::size_t>() );
  BOOST_CHECK( pos == chan.end() );
  for(std::size_t w=0; w< array.getNumRegularSpeakers();w++){
    std::size_t ch = array.getSpeakerChannel( w );
    //std::cout<<"w: "<<w<< " chn: "<< array.getSpeakerChannel( w )<<" spk: "<< array.getSpeakerIndexFromChn(ch+1)<<std::endl;
    BOOST_CHECK( w == array.getSpeakerIndexFromChn(ch));
  }

  std::size_t numberOfSources = 8;
  std::vector<XYZ> sourcePos( numberOfSources );
  //    sourcePos[0].set(-1.0f,	0.0, 0.0f, true);     // plane wave from front/back
  //    sourcePos[0].set(0.0f, -1.0f, 0.0f, true);     // plane wave from left/right
  //    sourcePos[0].set(0.0f, 0.0f, -1.0f, true);     // plane wave from below
  //    sourcePos[0].set(1.78f, 1.73f, -0.86f, false);

  //    vbap.setNumSources( numberOfSources );
  //    sourcePos[0].set(1.0, 1.0, 1.0, false);
  //    sourcePos[0].set(1.0,	0.3, -0.2, false); // for 9.1_audiolab.txt jumps between triplet 11 and 10 as z reduced
  //    sourcePos[0].set(0.0, 1.0, -0.9, false);
  
  std::tuple<float,float,float> pos0 = visr::efl::spherical2cartesian(visr::efl::degree2radian(-40.0f), visr::efl::degree2radian(15.0f), 1.0f);
  float px0,py0,pz0;
  std::tie(px0, py0, pz0) = pos0;
//  sourcePos[0].set(px0, py0,pz0, false );
  
  sourcePos[0].set( 1, -0.25f, 0.2f, false ); // source position not normalised
//  sourcePos[0].set( 2.08f, 1.0f, -5.0f, true );
  sourcePos[1].set( 1.0f, 0.0f, 0.0f, false );
  sourcePos[2].set( 0.0f, 1.0f, 0.0f, false );
  sourcePos[3].set( 0.0f, 0.0f, 1.0f, false );
  sourcePos[4].set( -1.0f, 0.0f, 0.0f, false );
  sourcePos[5].set( 0.0f, -1.0f, 0.0f, false );
  sourcePos[6].set( 0.0f, 0.0f, -1.0f, false );
  sourcePos[7].set( -1.0f, -1.0f, -1.0f, false );


  VBAP vbap( array, 0.0f, 0.0f, 0.0f );
  std::vector<SampleType> vbapGains;
  vbapGains.resize( array.getNumRegularSpeakers() );

  for(size_t i =0; i<sourcePos.size();i++){
    std::fill( vbapGains.begin(), vbapGains.end(), 0.0f );
    vbap.calculateGains( sourcePos[i].x, sourcePos[i].y, sourcePos[i].z, vbapGains.data() );
    //vbap.calculateGains( 1.0f, 1.0f, 0.0f, vbapGains.data() );
    std::size_t const numCols = array.getNumRegularSpeakers();

//    std::cout << "VBAP calculated loudspeaker gains with source's position ("<< sourcePos[i].x<<", " << sourcePos[i].y << ", " << sourcePos[i].z << ") : ";
    std::copy( vbapGains.data(), vbapGains.data() + numCols, std::ostream_iterator<Afloat>( std::cout, "\t\t" ) );
    std::cout << std::endl;
  }



  
  
  
 

 //BOOST_CHECK_NO_THROW( vbap.setLoudspeakerArray( &array ) );
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
    for( std::size_t k = 0; k < 9; k++ )
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
