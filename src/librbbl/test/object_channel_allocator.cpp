/* Copyright Institue of Sound and Vibration Research - All rights reserved. */

#include <librbbl/object_channel_allocator.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <iostream>
#include <random>

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

BOOST_AUTO_TEST_CASE( RepeatedAllocation )
{
  // Repeatedly allocate and deallocate the same vector.
  std::size_t const numChannels = 5;
  ObjectChannelAllocator objAlloc( numChannels );

  std::vector<objectmodel::ObjectId> const vec1 = {};
  std::vector<objectmodel::ObjectId> const vec2 = { { 0, 1, 2, 3 } };

  std::random_device rd;
  std::mt19937 gen( rd( ) );
  std::uniform_int_distribution<int> dis( 0, 1 );

  std::size_t const numTrials( 20000 );

  // BOOST_CHECK_NO_THROW( objAlloc.setObjects( vec1 ) );
  // BOOST_CHECK( objAlloc.numberUsedChannels( ) == vec1.size( ) );

  for( std::size_t runIdx( 0 ); runIdx < numTrials; ++runIdx )
  {
    int const choice = dis( gen );
    std::vector<objectmodel::ObjectId> const & v = choice == 0 ? vec1 : vec2;

    BOOST_CHECK_NO_THROW( objAlloc.setObjects( v ) );
    BOOST_CHECK( objAlloc.numberUsedChannels( ) == v.size( ) );
  }
}

BOOST_AUTO_TEST_CASE( RepeatedRandomAllocation )
{
  // Assign a large number of random-length random-element object vectors and 
  // check for the stability of object->channel assignments during consecutive iterations
  // if the object id is kept.
  std::size_t const numTrials = 20000;

  std::size_t const numChannels = 32;
  objectmodel::ObjectId const maxObjectId = numChannels;
  std::size_t numObjectsPerTrial = 64; // max. number of objects before removal of duplicates.

  ObjectChannelAllocator objAlloc( numChannels );

  std::vector<objectmodel::ObjectId> previousVector = {};

  using ChannelLookup = std::map<objectmodel::ObjectId, std::size_t>;

  ChannelLookup previousLookup;
  ChannelLookup currentLookup;

  std::random_device rd;
  std::mt19937 gen( rd( ) );

  std::uniform_int_distribution<std::size_t> lenGen( 0, maxObjectId );
  std::uniform_int_distribution<objectmodel::ObjectId> idGen( 0, maxObjectId );


  for( std::size_t trialIdx(0); trialIdx < numTrials; ++trialIdx )
  {
    previousLookup.swap( currentLookup );

    using IdVec = std::vector<objectmodel::ObjectId>;
    std::size_t vecLen = lenGen( gen );
    IdVec rawIds( vecLen );
    // Generate a sequence of random length with random, unique entries.
    std::generate( rawIds.begin(), rawIds.end(), [&]() { return idGen( gen ); } );
    std::sort( rawIds.begin(), rawIds.end() );
    IdVec::const_iterator endUniqueIt = std::unique( rawIds.begin(), rawIds.end() );
    rawIds.erase( endUniqueIt, rawIds.end() );

    BOOST_CHECK_NO_THROW( objAlloc.setObjects( rawIds ) );
    BOOST_CHECK( objAlloc.numberUsedChannels( ) == rawIds.size( ) );

    currentLookup.clear();
    for( std::size_t chIdx( 0 ); chIdx < objAlloc.maxChannels(); ++chIdx )
    {
      objectmodel::ObjectId const id = objAlloc.getObjectForChannel( chIdx );
      if( id != objectmodel::Object::cInvalidObjectId )
      {
        currentLookup.insert( std::make_pair( id, chIdx ) );
      }
    }

    // Check whether all channel assignments retained from the previous round kept the 
    // same channel indices.
    for( ChannelLookup::value_type const & v : currentLookup )
    {
      ChannelLookup::const_iterator findIt = previousLookup.find( v.first );
      if( findIt != previousLookup.end() )
      {
        BOOST_CHECK_MESSAGE( v.second == findIt->second, "Channel ID for same object changebetween iterations." );
      }
    }

  }
}

} // namespace test
} // namespace rbbl
} // namespace visr
