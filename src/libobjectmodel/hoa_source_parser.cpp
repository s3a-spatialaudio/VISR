/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "hoa_source_parser.hpp"

#include "hoa_source.hpp"

#include <boost/property_tree/ptree.hpp>

namespace visr
{
namespace objectmodel
{

/*virtual*/ void HoaSourceParser::
parse( boost::property_tree::ptree const & tree, Object & src ) const
{
  // parse generic part of object (parent of HoaSource)
  ObjectParser::parse( tree, src );

  try
  {
    // note: cannot check for object type id since src might be a subclass of HoaSource
    HoaSource & hoaSrc = dynamic_cast<HoaSource&>(src);

    // parse point source-specific data members
    hoaSrc.setOrder( tree.get<HoaSource::Order>( "order" ) );
  }
  // TODO: distinguish between boost property_tree parse errors and bad dynamic casts.
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing HOA source: object: ") + ex.what() );
  }
}

/*virtual*/ void HoaSourceParser::
write( Object const & obj, boost::property_tree::ptree & tree ) const
{
  // note: cannot check for object type id since obj might be a subclass of HoaSource
  HoaSource const& hoaObj = dynamic_cast<HoaSource const&>(obj);

  ObjectParser::write( obj, tree );
  tree.put<HoaSource::Order>( "order", hoaObj.order() );
}

} // namespace objectmodel
} // namespace visr
