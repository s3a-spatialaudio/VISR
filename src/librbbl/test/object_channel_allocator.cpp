/* Copyright Institue of Sound and Vibration Research - All rights reserved. */

#include <librbbl/object_channel_allocator.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>

namespace visr
{
namespace rbbl
{
namespace test
{

BOOST_AUTO_TEST_CASE( ObjectInitialization )
{
  std::size_t const numChannels = 3;
  ObjectChannelAllocator objAlloc( numChannels );

  BOOST_CHECK( objAlloc.maxChannels() == numChannels );
  BOOST_CHECK( objAlloc.numberUsedChannels() == 0 );
}

BOOST_AUTO_TEST_CASE( ObjectFilling )
{
  std::size_t const numChannels = 5;
  ObjectChannelAllocator objAlloc( numChannels );

  std::vector<objectmodel::ObjectId> initVec = { 7, 3, 5 };
  std::vector<objectmodel::ObjectId> initVecSorted( initVec );
  std::sort( initVecSorted.begin(), initVecSorted.end() );


  BOOST_CHECK_NO_THROW( objAlloc.setObjects( initVec ) );
  BOOST_CHECK( objAlloc.numberUsedChannels( ) == initVec.size() );

  // The channels should be allocated in object order.
  for( std::size_t chIdx( 0 ); chIdx < numChannels; ++chIdx )
  {
    objectmodel::ObjectId const objId = objAlloc.getObjectForChannel( chIdx );
    if( chIdx < initVec.size() )
    {
      BOOST_CHECK( objId == initVecSorted[chIdx] );
    }
    else
    {
      BOOST_CHECK( objId == objectmodel::Object::cInvalidObjectId );
    }
  }
}

BOOST_AUTO_TEST_CASE( ObjectPartialReplace )
{
  std::size_t const numChannels = 5;
  ObjectChannelAllocator objAlloc( numChannels );

  std::vector<objectmodel::ObjectId> initVec = { 3, 5, 7 };
  std::vector<objectmodel::ObjectId> initVecSorted( initVec );
  std::sort( initVecSorted.begin( ), initVecSorted.end( ) );

  BOOST_CHECK_NO_THROW( objAlloc.setObjects( initVec ) );
  BOOST_CHECK( objAlloc.numberUsedChannels( ) == initVec.size( ) );

  // Setting the same vector should not change anything
  BOOST_CHECK_NO_THROW( objAlloc.setObjects( initVec ) );
  BOOST_CHECK( objAlloc.numberUsedChannels( ) == initVec.size( ) );

  // Set a vector with the same number of channels, 
  std::vector<objectmodel::ObjectId> newVec = { 8, 5, 7 };
  BOOST_CHECK_NO_THROW( objAlloc.setObjects( newVec ) );
  BOOST_CHECK( objAlloc.numberUsedChannels( ) == newVec.size( ) );

  std::vector<objectmodel::ObjectId> newVecExpectedOrder = { 8, 5, 7 };

  for( std::size_t chIdx( 0 ); chIdx < numChannels; ++chIdx )
  {
    objectmodel::ObjectId const objId = objAlloc.getObjectForChannel( chIdx );
    if( chIdx < initVec.size( ) )
    {
      BOOST_CHECK( objId == newVecExpectedOrder[chIdx] );
    }
    else
    {
      BOOST_CHECK( objId == objectmodel::Object::cInvalidObjectId );
    }
  }

  std::vector<objectmodel::ObjectId> twoVec = { 5, 7 };
  BOOST_CHECK_NO_THROW( objAlloc.setObjects( twoVec ) );
  BOOST_CHECK( objAlloc.numberUsedChannels( ) == twoVec.size( ) );

}

BOOST_AUTO_TEST_CASE( ObjectCompleteReplace )
{
  std::size_t const numChannels = 5;
  ObjectChannelAllocator objAlloc( numChannels );

  std::vector<objectmodel::ObjectId> initVec = { 3, 5, 7 };
  std::vector<objectmodel::ObjectId> initVecSorted( initVec );
  std::sort( initVecSorted.begin( ), initVecSorted.end( ) );

  BOOST_CHECK_NO_THROW( objAlloc.setObjects( initVec ) );
  BOOST_CHECK( objAlloc.numberUsedChannels( ) == initVec.size( ) );

  // Set a completely different vector
  std::vector<objectmodel::ObjectId> newVec = { 12, 11, 10, 9 };
  BOOST_CHECK_NO_THROW( objAlloc.setObjects( newVec ) );
  BOOST_CHECK( objAlloc.numberUsedChannels( ) == newVec.size( ) );

  // Note: The allocation order depends on the history and can be quite complicated.

  // Set a smaller vector
  std::vector<objectmodel::ObjectId> twoVec = { 5, 7 };
  BOOST_CHECK_NO_THROW( objAlloc.setObjects( twoVec ) );
  BOOST_CHECK( objAlloc.numberUsedChannels( ) == twoVec.size( ) );
}

BOOST_AUTO_TEST_CASE( ObjectVectorClear )
{
  std::size_t const numChannels = 5;
  ObjectChannelAllocator objAlloc( numChannels );

  std::vector<objectmodel::ObjectId> initVec = { 3, 5, 7 };
  std::vector<objectmodel::ObjectId> initVecSorted( initVec );
  std::sort( initVecSorted.begin( ), initVecSorted.end( ) );

  BOOST_CHECK_NO_THROW( objAlloc.setObjects( initVec ) );
  BOOST_CHECK( objAlloc.numberUsedChannels( ) == initVec.size( ) );

  // Setting an empty vector should clear the allocator.
  std::vector<objectmodel::ObjectId> emptyVec = {};
  BOOST_CHECK_NO_THROW( objAlloc.setObjects( emptyVec ) );
  BOOST_CHECK( objAlloc.numberUsedChannels( ) == 0 );
}



} // namespace test
} // namespace rbbl
} // namespace visr
