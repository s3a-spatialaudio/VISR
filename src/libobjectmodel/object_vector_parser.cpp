/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "object_vector_parser.hpp"

#include "object_type.hpp"
#include "object_vector.hpp"
#include "object_factory.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sstream>
#include <stdexcept>

namespace visr
{
namespace objectmodel
{

/*static*/ void ObjectVectorParser::
fillObjectVector( std::string const & message, ObjectVector & res )
{
  return fillObjectVector( std::istringstream( message ), res );
}

/*static*/ void ObjectVectorParser::
fillObjectVector( std::basic_istream<char> & message, ObjectVector & res )
{
  using ptree = boost::property_tree::ptree;

  ptree propTree;
  try
  {
    read_json( message, propTree );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing a json object message: " ) + ex.what() );
  }

  try
  {
    // properties common to all source types
    std::string const objTypeStr = propTree.get<std::string>( "type" );

    // might throw (if the string does not match a recognised object type
    ObjectTypeId const objTypeId = stringToObjectType( objTypeStr );
    ObjectId const objId = propTree.get<ObjectId>( "id" );
    GroupId const groupId = propTree.get<GroupId>( "group" );
    LevelType const level = propTree.get<LevelType>( "level" );

    // TODO: either instantiate new object or cast existing object to matching type.
    // Proceed with type-dependent parsing in both cases.

    std::unique_ptr< Object > newObj( ObjectFactory::create(objTypeId) );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing the json message content: " ) + ex.what( ) );
  }

}

/*static*/ void ObjectVectorParser::
fillObjectVector( char const * message, ObjectVector & res )
{
  return fillObjectVector( std::string(message), res );
}

/*static*/ void ObjectVectorParser::
updateObjectVector( std::string const & message, ObjectVector & res )
{
  return updateObjectVector( std::istringstream( message ), res );
}

/*static*/ void ObjectVectorParser::
updateObjectVector( std::basic_istream<char> & message, ObjectVector & res )
{
  ObjectVector newVec;
  // todo: Check whether we can use the swap trick to use a single implementation
  // for fill and update
}

/*static*/ void ObjectVectorParser::
updateObjectVector( char const * message, ObjectVector & res )
{
  return updateObjectVector( std::string( message ), res );
}

} // namespace objectmodel
} // namespace visr
