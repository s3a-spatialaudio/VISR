/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/object.hpp>
#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/object_vector_parser.hpp>
#include <libobjectmodel/point_source.hpp>

#define BOOST_TEST_DYN_LINK
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
  std::string const msg = "{\"id\": 1,\n \"type\": \"point\",\n \"group\": 0,\n \"level\": 0.8,\n"
      "\"position\": {\"x\": 2.4, \"y\": 1.2,\n \"z\": -0.3 } }";

  ObjectVector scene;

  ObjectVectorParser::fillObjectVector( msg, scene );

}

} // namespace test
} // namespace objectmodel
} // namespce visr
