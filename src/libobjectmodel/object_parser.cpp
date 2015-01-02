/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "object_parser.hpp"

#include <boost/property_tree/ptree.hpp>

namespace visr
{
namespace objectmodel
{

/*virtual*/ void ObjectParser
::parse( boost::property_tree::ptree const & tree, Object & obj ) const
{
  obj.setObjectId( tree.get<ObjectId>( "id" ) );
  obj.setGroupId( tree.get<GroupId>( "group" ) );
  obj.setLevel( tree.get<LevelType>( "level" ) );
  obj.setPriority( tree.get<Object::Priority>( "priority" ) );
}

} // namespace objectmodel
} // namespace visr
