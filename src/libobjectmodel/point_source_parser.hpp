/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_POINT_SOURCE_PARSER_HPP_INCLUDED
#define VISR_OBJECTMODEL_POINT_SOURCE_PARSER_HPP_INCLUDED

#include "object_parser.hpp"

#include "object.hpp"

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class PointSourceParser: public ObjectParser
{
public:
  virtual void parse( boost::property_tree::ptree const & tree, Object & src ) const;

  virtual void write( Object const & obj, boost::property_tree::ptree & tree ) const;

protected:
  /**
   * Parse the content of a "position" node.
   * Supports Cartesian and spherical coordinates as well as default values for the z coordinate and the radius, respectively.
   * This is separated into a method because derived classes potentially need this functionality in other places.
   * @param posTree The property tree containing the coordinates (typically named "position")
   * @param x [out] Reference to hold the parsed x coordinate.
   * @param z [out] Reference to hold the parsed z coordinate.
   * @todo Consider making this a more general facility.
   */
  static void parsePosition( boost::property_tree::ptree const & posTree, 
                             Object::Coordinate & x,
                             Object::Coordinate & y, 
                             Object::Coordinate & z );

};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_POINT_SOURCE_PARSER_HPP_INCLUDED
