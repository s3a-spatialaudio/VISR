/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "point_source_with_diffuseness_parser.hpp"

#include "point_source_parser.hpp"
#include "point_source_with_diffuseness.hpp"

#include <boost/property_tree/ptree.hpp>

namespace visr
{
namespace objectmodel
{

/*virtual*/ void PointSourceWithDiffusenessParser::
parse( boost::property_tree::ptree const & tree, Object & src ) const
{
  try
  {
    // note: cannot check for object type id since src might be a subclass of PointSource
    PointSourceWithDiffuseness & diffusePointSrc = dynamic_cast<PointSourceWithDiffuseness&>(src);

    // Parse all members inherited from the base class PointSource
    PointSourceParser::parse( tree, diffusePointSrc );

    // parse point source-specific data members
    diffusePointSrc.setDiffuseness( tree.get<PointSource::Coordinate>( "diffuseness" ) );
  }
  // TODO: distinguish between boost property_tree parse errors and bad dynamic casts.
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing point source: object: ") + ex.what() );
  }
}

/*virtual*/ void PointSourceWithDiffusenessParser::
write( Object const & obj, boost::property_tree::ptree & tree ) const
{
  // note: cannot check for object type id since obj might be a subclass of PointSource
  PointSourceWithDiffuseness const& pswdObj = dynamic_cast<PointSourceWithDiffuseness const&>(obj);

  PointSourceParser::write( obj, tree );
  tree.put<PointSource::Coordinate>( "diffuseness", pswdObj.diffuseness() );
}

} // namespace objectmodel
} // namespace visr
