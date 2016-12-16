/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "object_parser.hpp"
#include "object_type.hpp"

#include <libpml/index_sequence.hpp>

#include <boost/property_tree/ptree.hpp>

#include <iostream>
#include <sstream>

namespace visr
{
namespace objectmodel
{

/*virtual*/ void ObjectParser
::parse( boost::property_tree::ptree const & tree, Object & obj ) const
{
  try
  {
    std::string const channelString = tree.get<std::string>( "channels" );
    pml::IndexSequence const channelIndices( channelString );
    // Todo: Consider more convenient set method.
    obj.resetNumberOfChannels( channelIndices.size() );
    for( std::size_t idx( 0 ); idx < obj.numberOfChannels(); ++idx )
    {
      obj.setChannelIndex( idx, channelIndices.at( idx ) );
    }
  }
  catch( std::exception const & ex )
  {
    std::cerr << "ObjectParser: Parsing of channel indices failed: " << ex.what() << std::endl;
  }

  obj.setObjectId( tree.get<ObjectId>( "id" ) );
  obj.setGroupId( tree.get<GroupId>( "group" ) );
  obj.setLevel( tree.get<LevelType>( "level" ) );
  obj.setPriority( tree.get<Object::Priority>( "priority" ) );
}

/*virtual*/ void ObjectParser
::write( Object const & obj, boost::property_tree::ptree & tree ) const
{
  tree.put( "type", objectTypeToString(obj.type( )) );
  std::size_t const numChannels( obj.numberOfChannels() );
  std::stringstream chString;
  for( std::size_t chIdx( 0 ); chIdx < numChannels; ++chIdx )
  {
    chString << obj.channelIndex( chIdx );
    if( chIdx < numChannels - 1 )
    {
      chString << ", ";
    }
  }
  tree.put( "channels", chString.str() );
  tree.put<ObjectId>( "id", obj.id() );
  tree.put<GroupId>( "group", obj.groupId() );
  tree.put<LevelType>( "level", obj.level() );
  tree.put<Object::Priority>( "priority", obj.priority() );
}

} // namespace objectmodel
} // namespace visr
