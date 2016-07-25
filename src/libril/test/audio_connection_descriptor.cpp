/* Copyright Institue of Sound and Vibration Research - All rights reserved. */


#include <libril/audio_connection_descriptor.hpp>

#include <boost/test/unit_test.hpp>

namespace visr
{
namespace ril
{
namespace test
{

BOOST_AUTO_TEST_CASE( instantiateAudioChannelSlice )
{
  ril::AudioChannelSlice const emptySlice;
  BOOST_CHECK( emptySlice.size() == 0 );

  std::size_t const singleVal( 42 );
  ril::AudioChannelSlice const singleValSlice( singleVal );
  BOOST_CHECK( singleValSlice.size() == 1 );
  BOOST_CHECK( singleValSlice.start() == singleVal );
  BOOST_CHECK( singleValSlice[0] == singleVal );
  BOOST_CHECK_THROW( singleValSlice.at( 1 ), std::out_of_range );

  ril::AudioChannelSlice const sliceFromInitList{ 10, 5, -1 };
  BOOST_CHECK( sliceFromInitList.size() == 5 );
  BOOST_CHECK( sliceFromInitList[2] == 8 );
}

BOOST_AUTO_TEST_CASE( instantiateAudioChannelIndexVector )
{
  ril::AudioChannelIndexVector const emptyVec;
  BOOST_CHECK( emptyVec.size() == 0 );

  std::vector<std::size_t> initVec{ 3, 4, 5, 6, 7 };
  ril::AudioChannelIndexVector const vecFromIndexVec( initVec );
  BOOST_CHECK( vecFromIndexVec.size() == initVec.size() );

  ril::AudioChannelIndexVector const vecFromInitList{ 3, 4, 5, 6, 7 };
  BOOST_CHECK( vecFromInitList.size( ) == initVec.size( ) );

  ril::AudioChannelIndexVector const vecFromSlice( AudioChannelSlice( 10, 5, -1 ) );
  BOOST_CHECK( vecFromSlice.size() == 5 );
  BOOST_CHECK( vecFromSlice[4] == 6 );

  ril::AudioChannelIndexVector const vecFromMultipleSlices( { AudioChannelSlice( 10, 5, -1 ), AudioChannelSlice( 0, 5, 1 ) } );
  BOOST_CHECK( vecFromMultipleSlices.size( ) == 10 );
  BOOST_CHECK( vecFromMultipleSlices[6] == 1 );

}



} // namespace test
} // namespace ril
} // namespace visr
