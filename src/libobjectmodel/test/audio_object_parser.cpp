/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/object.hpp>
#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/object_vector_parser.hpp>
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

BOOST_AUTO_TEST_CASE( ParsePointSource )
{
  std::string const msg = "{ \"objects\":[{\"id\": 1, \"channels\": \"0\", \n \"type\": \"point\",\n \"group\": 0,\n \"priority\": 3,\n \"level\": 0.8,\n"
      "\"position\": {\"x\": 2.4, \"y\": 1.2,\n \"z\": -0.3 } }, {\"id\": 5,\"channels\": \"1\",\n \"type\": \"point\",\n \"group\": 1,\n \"priority\": 2,\n \"level\": 0.375,\n"
      "\"position\": {\"x\": 1.5, \"y\": -1.5,\n \"z\": 0 } }] }";

  ObjectVector scene;

  ObjectVectorParser::fillObjectVector( msg, scene );

  BOOST_CHECK( scene.size() == 2 );
}

BOOST_AUTO_TEST_CASE( ParsePlaneWave )
{
  std::string const msg = "{\"objects\": [ {\"id\": 27,\n \"channels\": \"0\",\n\"type\": \"plane\",\n \"group\":7,\n \"priority\": 3,\n \"level\": 0.325,\n"
    "\"direction\": {\"az\": 0.5, \"el\": 0.9,\n \"refdist\": 0.75 } } ] }";

  ObjectVector scene;

  ObjectVectorParser::fillObjectVector( msg, scene );

  BOOST_CHECK( scene.size() == 1 );
}

BOOST_AUTO_TEST_CASE( UpdateSceneSameIdSameType )
{
  std::string const msg = "{\"objects\":[ {\"id\": 27,\n \"channels\": \"0\",\n \"type\": \"plane\",\n \"group\":7,\n \"priority\": 3,\n \"level\": 0.325,\n"
    "\"direction\": {\"az\": 0.5, \"el\": 0.9,\n \"refdist\": 0.75 } } ] }";

  ObjectVector scene;

  ObjectVectorParser::fillObjectVector( msg, scene );

  BOOST_CHECK( scene.size() == 1 );

  std::string const msg2 = "{\"objects\": [ {\"id\": 27,\n \"channels\": \"0\",\n \"type\": \"plane\",\n \"group\":12,\n \"priority\": 5,\n \"level\": 0.25,\n"
    "\"direction\": {\"az\": 1.2, \"el\": 0.0,\n \"refdist\": 1.25 } } ] }";

  ObjectVectorParser::updateObjectVector( msg2, scene );

  BOOST_CHECK( scene.size() == 1 );

  ObjectId const id = 27;

  Object const & retrievedObj = scene.at( id );

  BOOST_CHECK( retrievedObj.groupId() == 12 );

  BOOST_CHECK_CLOSE( retrievedObj.level(), 0.25f, 1e-6f );
}

BOOST_AUTO_TEST_CASE( UpdateSceneSameIdDifferentType )
{
  std::string const msg = "{ \"objects\":[{\"id\": 27,\n \"channels\": \"0\",\n \"type\": \"point\",\n \"group\": 0,\n \"priority\": 3,\n \"level\": 0.8,\n"
      "\"position\": {\"x\": 2.4, \"y\": 1.2,\n \"z\": -0.3 } }, {\"id\": 5,\n \"channels\": \"0\",\n \"type\": \"point\",\n \"group\": 1,\n \"priority\": 2,\n \"level\": 0.375,\n"
      "\"position\": {\"x\": 1.5, \"y\": -1.5,\n \"z\": 0 } }] }";

  ObjectVector scene;

  ObjectVectorParser::fillObjectVector( msg, scene );

  BOOST_CHECK( scene.size() == 2 );

  std::string const msg2 = "{\"objects\": [ {\"id\": 27,\n \"channels\": \"0\",\n \"type\": \"plane\",\n \"group\":12,\n \"priority\": 5,\n \"level\": 0.25,\n"
    "\"direction\": {\"az\": 1.2, \"el\": 0.0,\n \"refdist\": 1.25 } } ] }";

  ObjectVectorParser::updateObjectVector( msg2, scene );

  BOOST_CHECK( scene.size() == 2 );

  ObjectId const id = 27;

  Object const & retrievedObj = scene.at( id );

  BOOST_CHECK( retrievedObj.groupId() == 12 );

  BOOST_CHECK_CLOSE( retrievedObj.level(), 0.25f, 1e-6f );
}

BOOST_AUTO_TEST_CASE( ParseMultiChannelObject )
{
  std::string const msg = "{ \"objects\":[{\"id\": 27,\n \"channels\": \"0,1,2:4\",\n \"type\": \"point\",\n \"group\": 0,\n \"priority\": 3,\n \"level\": 0.8,\n"
    "\"position\": {\"x\": 2.4, \"y\": 1.2,\n \"z\": -0.3 } }, {\"id\": 5,\n \"channels\": \"0\",\n \"type\": \"point\",\n \"group\": 1,\n \"priority\": 2,\n \"level\": 0.375,\n"
    "\"position\": {\"x\": 1.5, \"y\": -1.5,\n \"z\": 0 } }] }";

  ObjectVector scene;

  ObjectVectorParser::fillObjectVector( msg, scene );

  BOOST_CHECK( scene.size( ) == 2 );

  ObjectId const id = 27;

  Object const & retrievedObj = scene.at( id );

  BOOST_CHECK( retrievedObj.groupId( ) == 12 );

  BOOST_CHECK( retrievedObj.numberOfChannels() == 5 );
}


} // namespace test
} // namespace objectmodel
} // namespce visr
