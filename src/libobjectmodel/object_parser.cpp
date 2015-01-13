/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "object_parser.hpp"

#include <boost/property_tree/ptree.hpp>

#include<iostream>

namespace visr
{
namespace objectmodel
{

/*virtual*/ void ObjectParser
::parse( boost::property_tree::ptree const & tree, Object & obj ) const
{
  // TODO: extend this to multichannel objects using a syntax like "0 1 2", "0,1,2" with arbitrary spacing or even "0-2,3 4-6"
  try
  {
  // for the moment, we only support monaural channels
  Object::ChannelIndex const scalarChannelIndex = tree.get<Object::ChannelIndex>( "channels" );
  obj.resetNumberOfChannels( 1 );
  obj.setChannelIndex( 0, scalarChannelIndex );
  }
  catch( std::exception const & ex )
  {
    std::cerr << "ObjectParser: Parsing of channel index failed (only single-channel objects are supported at the moment)";
  }

  obj.setObjectId( tree.get<ObjectId>( "id" ) );
  obj.setGroupId( tree.get<GroupId>( "group" ) );
  obj.setLevel( tree.get<LevelType>( "level" ) );
  obj.setPriority( tree.get<Object::Priority>( "priority" ) );
}

} // namespace objectmodel
} // namespace visr
