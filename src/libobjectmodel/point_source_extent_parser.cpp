/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "point_source_extent_parser.hpp"

#include "point_source_parser.hpp"
#include "point_source_extent.hpp"

#include <boost/property_tree/ptree.hpp>

namespace visr
{
namespace objectmodel
{

/*virtual*/ void PointSourceExtentParser::
parse( boost::property_tree::ptree const & tree, Object & src ) const
{
  try
  {
    // note: cannot check for object type id since src might be a subclass of PointSource
    PointSourceExtent & extentPointSrc = dynamic_cast<PointSourceExtent&>(src);

    // Parse all members inherited from the base class PointSource
    PointSourceParser::parse( tree, extentPointSrc );

    // parse data members specific to extent sources
    extentPointSrc.setWidth( tree.get<PointSource::Coordinate>( "width" ) );
    extentPointSrc.setHeight( tree.get<PointSource::Coordinate>( "height" ) );
    extentPointSrc.setDepth( tree.get<PointSource::Coordinate>( "depth", 0.0 ) );
  }
  // TODO: distinguish between boost property_tree parse errors and bad dynamic casts.
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing point source with extent object: ") + ex.what() );
  }
}

/*virtual*/ void PointSourceExtentParser::
write( Object const & obj, boost::property_tree::ptree & tree ) const
{
  // note: cannot check for object type id since obj might be a subclass of PointSource
  PointSourceExtent const& pseObj = dynamic_cast<PointSourceExtent const&>(obj);

  PointSourceParser::write( obj, tree );
  tree.put<PointSource::Coordinate>( "width", pseObj.width() );
  tree.put<PointSource::Coordinate>( "height", pseObj.height() );
  tree.put<PointSource::Coordinate>( "depth", pseObj.depth() );
}

} // namespace objectmodel
} // namespace visr
