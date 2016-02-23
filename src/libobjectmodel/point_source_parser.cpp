/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "point_source_parser.hpp"

#include "point_source.hpp"

#include <libefl/cartesian_spherical_conversion.hpp>
#include <libefl/degree_radian_conversion.hpp>

#include <boost/property_tree/ptree.hpp>

#include <ciso646>

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

    PointSource::Coordinate xPos, yPos, zPos;
    parsePosition( tree.get_child( "position"), xPos, yPos, zPos );
    pointSrc.setX( xPos );
    pointSrc.setY( yPos );
    pointSrc.setZ( zPos );
  }
  // TODO: distinguish between boost property_tree parse errors and bad dynamic casts.
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing point source: object: ") + ex.what() );
  }
}

/*virtual*/ void PointSourceParser::
write( Object const & obj, boost::property_tree::ptree & tree ) const
{
  // note: cannot check for object type id since obj might be a subclass of PointSource
  PointSource const& psObj = dynamic_cast<PointSource const&>(obj);

  ObjectParser::write( obj, tree );
  tree.put<PointSource::Coordinate>( "position.x", psObj.x() );
  tree.put<PointSource::Coordinate>( "position.y", psObj.y() );
  tree.put<PointSource::Coordinate>( "position.z", psObj.z() );
}

/*static*/ void 
PointSourceParser::parsePosition( boost::property_tree::ptree const & posTree,
                                  Object::Coordinate & x,
                                  Object::Coordinate & y,
                                  Object::Coordinate & z )
{
  // Parse point source-specific data members
  // Allow both cartesian and spherical coordinates, but no mixture between them
  boost::optional<PointSource::Coordinate> const coordX = posTree.get_optional<PointSource::Coordinate>( "x" );
  boost::optional<PointSource::Coordinate> const coordY = posTree.get_optional<PointSource::Coordinate>( "y" );
  boost::optional<PointSource::Coordinate> const coordZ = posTree.get_optional<PointSource::Coordinate>( "z" );
  boost::optional<PointSource::Coordinate> const coordAz = posTree.get_optional<PointSource::Coordinate>( "az" );
  boost::optional<PointSource::Coordinate> const coordEl = posTree.get_optional<PointSource::Coordinate>( "el" );
  boost::optional<PointSource::Coordinate> const coordRadius = posTree.get_optional<PointSource::Coordinate>( "radius" );

  bool const anyCartCoord = (coordX or coordY or coordZ);
  bool const anySphCoord = (coordAz or coordEl or coordRadius);

  if( not( anyCartCoord or anySphCoord ) )
  {
    throw std::invalid_argument( "Positions contain either Cartesian or spherical coordinates" );
  }
  if( anyCartCoord and anySphCoord )
  {
    throw std::invalid_argument( "Positions must not contain both Cartesian or spherical coordinates" );
  }
  if( anyCartCoord )
  {
    if( not( coordX and coordY ) )
    {
      throw std::invalid_argument( "Positions in Cartesian coordinates must provide the \"x\" and \"y\" attributes." );
    }
    x = *coordX;
    y = *coordY;
    z = coordZ ? *coordZ : static_cast<PointSource::Coordinate>(0.0);
  }
  else // use spherical coordinates
  {
    if( not( coordAz and coordEl ) )
    {
      throw std::invalid_argument( "Positions in spherical coordinates must provide the \"az\" and \"el\" attributes." );
    }
    PointSource::Coordinate const radius = coordRadius ? *coordRadius : static_cast<PointSource::Coordinate>(1.0);
    std::tie( x, y, z ) = efl::spherical2cartesian( efl::degree2radian( *coordAz ),
                                                    efl::degree2radian( *coordEl ),
                                                    radius );
  }
}

} // namespace objectmodel
} // namespace visr
