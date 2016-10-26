/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "channel_object_parser.hpp"

#include "channel_object.hpp"

#include <libpml/index_sequence.hpp>

#include <boost/property_tree/ptree.hpp>

#include <algorithm>
#include <iterator>

namespace visr
{
namespace objectmodel
{

/*virtual*/ void ChannelObjectParser::
parse( boost::property_tree::ptree const & tree, Object & obj ) const
{
  try
  {
    // parse generic part of Object (parent of ChannelObject)
    ObjectParser::parse( tree, obj );

    std::string const outChannelStr = tree.get<std::string>( "outputChannels" );
    pml::IndexSequence const outChannels{ outChannelStr };

    if( outChannels.size() != obj.numberOfChannels())
    {
      throw std::invalid_argument("ChannelObjectParser::parse() Number of output channels does not match the number of object channels.");
    }
    ChannelObject & chObj = dynamic_cast<ChannelObject &>(obj);
    chObj.setOutputChannels( &outChannels[0], outChannels.size() );
  }
  // TODO: distinguish between boost property_tree parse errors and bad dynamic casts.
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing channel object: ") + ex.what() );
  }
}

/*virtual*/ void ChannelObjectParser
::write( Object const & obj, boost::property_tree::ptree & tree ) const
{
  ObjectParser::write( obj, tree );

  ChannelObject const & chObj = dynamic_cast<ChannelObject const &>(obj);

  // Check object invariant before writing
  if( chObj.numberOfChannels() != chObj.outputChannels().size())
  {
    throw std::invalid_argument( "ChannelObjectParser::write(): Object is inconsistent, because the number of output channels does not the number of object channels." );
  }

  std::stringstream outChannelStr;
  static char * const cSeparator = ",";
  if (chObj.outputChannels().size() > 0)
  {
    std::copy(chObj.outputChannels().begin(), std::prev(chObj.outputChannels().end()),
      std::ostream_iterator<ChannelObject::OutputChannelId>(outChannelStr, cSeparator));
    outChannelStr << chObj.outputChannels().back();
  }
  tree.put( "outputChannels", outChannelStr.str() );
}

} // namespace objectmodel
} // namespace visr
