/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/audio_object.hpp>
#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/point_source.hpp>

#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <iostream>

namespace visr
{
namespace objectmodel
{
namespace test
{

BOOST_AUTO_TEST_CASE( InstantiatePointSources )
{
  std::cout << "InstantiatePointSources started...." << std::endl;


  ObjectVector scene;

  ObjectId const id1 = 1;
  PointSource s1( id1 );
  BOOST_CHECK_NO_THROW( scene.set( id1, s1 ) );

  BOOST_CHECK( scene.size() == 1 );

  ObjectId const id2 = 2;
  PointSource s2( id2 );
  BOOST_CHECK_NO_THROW( scene.set( id2, s2 ) );
  BOOST_CHECK( scene.size() == 2 );

  BOOST_CHECK_NO_THROW( scene.at( id1 ) );
  AudioObject const & retrievedObj1 = scene.at( id1 );

  BOOST_CHECK( s1.id() == retrievedObj1.id() );

  // attempt access to source with non-existing ID
  BOOST_CHECK_THROW( AudioObject const & retrievedObjInvld = scene.at( 5 ), std::exception );

  PointSource s3( id1 );

  BOOST_CHECK_NO_THROW( scene.set( id1, s3 ) ); // Replacing object at existing ID is legal
  BOOST_CHECK( scene.size() == 2 ); // s3 replaces s1 at id1, size remains unchanged


}

} // namespace test
} // namespace objectmodel
} // namespce visr
