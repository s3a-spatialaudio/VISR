/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "diffuse_source_parser.hpp"

#include "diffuse_source.hpp"

#include <boost/property_tree/ptree.hpp>

namespace visr
{
namespace objectmodel
{

/*virtual*/ void DiffuseSourceParser::
parse( boost::property_tree::ptree const & tree, Object & obj ) const
{
  try
  {
    // parse generic part of Object (parent of DiffuseSource)
    ObjectParser::parse( tree, obj );

    // Nothing estra to be performed.
  }
  // TODO: distinguish between boost property_tree parse errors and bad dynamic casts.
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing point source: object: ") + ex.what() );
  }
}

} // namespace objectmodel
} // namespace visr
