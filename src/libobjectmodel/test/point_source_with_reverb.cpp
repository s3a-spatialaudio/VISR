/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/object.hpp>
#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/object_vector_parser.hpp>
#include <libobjectmodel/point_source_with_reverb.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/test/unit_test.hpp>

#include <ciso646>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

namespace visr
{
namespace objectmodel
{
namespace test
{

BOOST_AUTO_TEST_CASE( ParsePointSourceWithReverb )
{
  boost::filesystem::path const jsonFileName = boost::filesystem::path( CMAKE_CURRENT_SOURCE_DIR ) / boost::filesystem::path("/data/room_object_1.json" );
  BOOST_CHECK( exists( jsonFileName ) and not is_directory( jsonFileName ) );
  std::ifstream jsonFileStr( jsonFileName.string().c_str() );
  BOOST_CHECK( jsonFileStr and not jsonFileStr.eof() );

  std::stringstream msgStr;
  msgStr << jsonFileStr.rdbuf();
  std::string const msg = msgStr.str();
  ObjectVector scene;

  ObjectVectorParser::fillObjectVector( msg, scene );


  BOOST_CHECK( scene.size() == 1 );
}

} // namespace test
} // namespace objectmodel
} // namespce visr
