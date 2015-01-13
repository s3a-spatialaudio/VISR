/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "plane_wave_parser.hpp"

#include "plane_wave.hpp"

#include <boost/property_tree/ptree.hpp>

namespace visr
{
namespace objectmodel
{

/*virtual*/ void PlaneWaveParser::
parse( boost::property_tree::ptree const & tree, Object & obj ) const
{
  // parse generic part of object (parent of PointSource)
  ObjectParser::parse( tree, obj );

  try
  {
    // note: cannot check for object type id since obj might be a subclass of PointSource
    PlaneWave & pwSrc = dynamic_cast<PlaneWave&>(obj);

    // parse point source-specific data members
    pwSrc.setIncidenceAzimuth( tree.get<Object::Coordinate>( "direction.az" ) );
    pwSrc.setIncidenceElevation( tree.get<Object::Coordinate>( "direction.el" ) );
    pwSrc.setReferenceDistance( tree.get<Object::Coordinate>( "direction.refdist" ) );
  }
  // TODO: distinguish between boost property_tree parse errors and bad dynamic casts.
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing point source: object: ") + ex.what() );
  }
}

} // namespace objectmodel
} // namespace visr
