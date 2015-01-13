/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "point_source_parser.hpp"

#include "point_source.hpp"

#include <boost/property_tree/ptree.hpp>

namespace visr
{
namespace objectmodel
{

/*virtual*/ void PointSourceParser::
parse( boost::property_tree::ptree const & tree, Object & src ) const
{
  // parse generic part of object (parent of PointSource)
  ObjectParser::parse( tree, src );

  try
  {
    // note: cannot check for object type id since src might be a subclass of PointSource
    PointSource & pointSrc = dynamic_cast<PointSource&>(src);

    // parse point source-specific data members
    pointSrc.setX( tree.get<PointSource::Coordinate>( "position.x" ) );
    pointSrc.setY( tree.get<PointSource::Coordinate>( "position.y" ) );
    pointSrc.setZ( tree.get<PointSource::Coordinate>( "position.z" ) );
  }
  // TODO: distinguish between boost property_tree parse errors and bad dynamic casts.
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing point source: object: ") + ex.what() );
  }
}

} // namespace objectmodel
} // namespace visr
