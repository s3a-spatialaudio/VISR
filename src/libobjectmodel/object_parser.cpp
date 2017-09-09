/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "object_parser.hpp"
#include "object_type.hpp"

#include <librbbl/index_sequence.hpp>

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
    rbbl::IndexSequence const channelIndices( channelString );
    // Todo: Consider more convenient set method.
    obj.resetNumberOfChannels( channelIndices.size() );
    for( std::size_t idx( 0 ); idx < obj.numberOfChannels(); ++idx )
    {
      obj.setChannelIndex( idx, static_cast<Object::ChannelIndex>( channelIndices.at( idx ) ) );
    }
  }
  catch( std::exception const & ex )
  {
    std::cerr << "ObjectParser: Parsing of channel indices failed: " << ex.what() << std::endl;
  }

  obj.setGroupId( tree.get<GroupId>( "group" ) );
  obj.setLevel( tree.get<LevelType>( "level" ) );
  obj.setPriority( tree.get<Object::Priority>( "priority" ) );

  rbbl::ParametricIirCoefficientList<Object::Coordinate> eqCoeffs;
  boost::property_tree::ptree::const_assoc_iterator eqIt =  tree.find( "eq");
  if( eqIt != tree.not_found() )
  {
    eqCoeffs.loadJson( eqIt->second );
  }
  obj.setEqCoefficients( eqCoeffs);
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

  if( !obj.eqCoefficients().empty() )
  {
    boost::property_tree::ptree eqNode;
    obj.eqCoefficients().writeXml( eqNode );
    tree.add_child( "eq", eqNode );
  }
}

} // namespace objectmodel
} // namespace visr
