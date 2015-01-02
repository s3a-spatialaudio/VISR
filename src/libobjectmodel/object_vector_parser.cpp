/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "object_vector_parser.hpp"

#include "object_factory.hpp"
#include "object_parser.hpp"
#include "object_type.hpp"
#include "object_vector.hpp"

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
  // Provide strong exception safety by adding the new elements to a new object 
  // vector and swapping the vectors at the end of the function.
  ObjectVector newVec;

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

    // Instantiate an object of the correct type
    std::unique_ptr< Object > newObj( ObjectFactory::create(objTypeId) );

    ObjectParser const & objParser = ObjectFactory::parser( objTypeId );

    objParser.parse( propTree, *newObj );

    newVec.set( newObj->id(), *newObj );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing the json message content: " ) + ex.what( ) );
  }
  newVec.swap( res );
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
