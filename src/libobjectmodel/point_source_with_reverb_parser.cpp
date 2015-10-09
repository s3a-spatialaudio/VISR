/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "point_source_with_reverb_parser.hpp"

#include "point_source_parser.hpp"
#include "point_source_with_reverb.hpp"

#include <boost/property_tree/ptree.hpp>

namespace visr
{
namespace objectmodel
{

/*virtual*/ void PointSourceWithReverbParser::
parse( boost::property_tree::ptree const & tree, Object & src ) const
{
  try
  {
    // note: cannot check for object type id since src might be a subclass of PointSource
    PointSourceWithReverb & reverbPointSrc = dynamic_cast<PointSourceWithReverb&>(src);

    // Parse all members inherited from the base class PointSource
    PointSourceParser::parse( tree, reverbPointSrc );

    // parse point source-specific data members
  }
  // TODO: distinguish between boost property_tree parse errors and bad dynamic casts.
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing point source with reverb: object: ") + ex.what() );
  }
}

/*virtual*/ void PointSourceWithReverbParser::
write( Object const & obj, boost::property_tree::ptree & tree ) const
{
  // note: cannot check for object type id since obj might be a subclass of PointSource
  PointSourceWithReverb const& pswdObj = dynamic_cast<PointSourceWithReverb const&>(obj);

  PointSourceParser::write( obj, tree );
}

} // namespace objectmodel
} // namespace visr
