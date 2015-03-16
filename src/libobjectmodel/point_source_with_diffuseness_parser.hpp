/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_POINT_SOURCE_PARSER_WITH_DIFFUSENESS_HPP_INCLUDED
#define VISR_OBJECTMODEL_POINT_SOURCE_PARSER_WITH_DIFFUSENESS_HPP_INCLUDED

#include "point_source_parser.hpp"

#include "object.hpp"

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class PointSourceWithDiffusenessParser: public PointSourceParser
{
public:
  virtual void parse( boost::property_tree::ptree const & tree, Object & src ) const;

  virtual void write( Object const & obj, boost::property_tree::ptree & tree ) const;
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_POINT_SOURCE_PARSER_WITH_DIFFUSENESS_HPP_INCLUDED
