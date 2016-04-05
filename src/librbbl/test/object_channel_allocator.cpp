/* Copyright Institue of Sound and Vibration Research - All rights reserved. */

#include <librbbl/object_channel_allocator.hpp>

#include <boost/test/unit_test.hpp>

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

  BOOST_CHECK_NO_THROW( objAlloc.setObjects( initVec ) );
  BOOST_CHECK( objAlloc.numberUsedChannels( ) == initVec.size() );

  // The channels should be allocated in object order.
  for( std::size_t chIdx( 0 ); chIdx < numChannels; ++chIdx )
  {
    objectmodel::ObjectId const objId = objAlloc.getObjectForChannel( chIdx );
    if( chIdx < initVec.size() )
    {
      BOOST_CHECK( objId == initVec[chIdx] );
    }
    else
    {
      BOOST_CHECK( objId == objectmodel::Object::cInvalidObjectId );
    }
  }
}


} // namespace test
} // namespace rbbl
} // namespace visr
