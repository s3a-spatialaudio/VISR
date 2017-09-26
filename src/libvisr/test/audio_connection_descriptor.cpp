/* Copyright Institue of Sound and Vibration Research - All rights reserved. */


#include <libvisr/channel_list.hpp>

#include <boost/test/unit_test.hpp>

namespace visr
{
namespace ril
{
namespace test
{

BOOST_AUTO_TEST_CASE( instantiateChannelRange )
{
  // Default constructor
  ChannelRange const emptyRange;
  BOOST_CHECK( emptyRange.size() == 0 );

  std::size_t const singleVal( 42 );
  ChannelRange const singleValRange( singleVal );
  BOOST_CHECK( singleValRange.size() == 1 );
  BOOST_CHECK( singleValRange.start() == singleVal );
  BOOST_CHECK( singleValRange[0] == singleVal );
  BOOST_CHECK_THROW( singleValRange.at( 1 ), std::out_of_range );

  ChannelRange const sliceFromInitList{ 1, 6, 2 };
  BOOST_CHECK( sliceFromInitList.size() == 3 );
  BOOST_CHECK( sliceFromInitList[2] == 5 );
}

BOOST_AUTO_TEST_CASE( instantiateAudioChannelIndexVector )
{
  ChannelList const emptyList;
  BOOST_CHECK( emptyList.size() == 0 );

  std::vector<std::size_t> initVec{ 3, 4, 5, 6, 7 };
  ChannelList const vecFromIndexVec( initVec );
  BOOST_CHECK( vecFromIndexVec.size() == initVec.size() );

  ChannelList const vecFromInitList{ 3, 4, 5, 6, 7 };
  BOOST_CHECK( vecFromInitList.size( ) == initVec.size( ) );

  ChannelList const vecFromRange( ChannelRange( 10, 5, -1 ) );
  BOOST_CHECK( vecFromRange.size() == 5 );
  BOOST_CHECK( vecFromRange[4] == 6 );

  ChannelList const vecFromMultipleRanges( { ChannelRange( 10, 5, -1 ), ChannelRange( 0, 5, 1 ) } );
  BOOST_CHECK( vecFromMultipleRanges.size( ) == 10 );
  BOOST_CHECK( vecFromMultipleRanges[6] == 1 );

  ChannelList const vecFromMultipleInitLists = { ChannelRange{ 10, 5, -1 }, ChannelRange{ 3, 7, 1 } };
  BOOST_CHECK( vecFromMultipleInitLists.size() == 9 );
  BOOST_CHECK( vecFromMultipleInitLists[5] == 3 );

}

} // namespace test
} // namespace ril
} // namespace visr
