/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_POINT_SOURCE_EXTENT_PARSER_HPP_INCLUDED
#define VISR_OBJECTMODEL_POINT_SOURCE_EXTENT_PARSER_HPP_INCLUDED

#include "point_source_with_diffuseness_parser.hpp"

#include "export_symbols.hpp"
#include "object.hpp"

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class VISR_OBJECTMODEL_LIBRARY_SYMBOL PointSourceExtentParser: public PointSourceWithDiffusenessParser
{
public:
  virtual void parse( boost::property_tree::ptree const & tree, Object & src ) const;

  virtual void write( Object const & obj, boost::property_tree::ptree & tree ) const;
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_POINT_SOURCE_EXTENT_PARSER_HPP_INCLUDED
