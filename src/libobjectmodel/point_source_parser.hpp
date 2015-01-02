/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_POINT_SOURCE_PARSER_HPP_INCLUDED
#define VISR_OBJECTMODEL_POINT_SOURCE_PARSER_HPP_INCLUDED

#include "point_source.hpp"

#include <boost/property_tree/ptree.hpp>

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class PointSourceParser
{
public:
  static void parse( boost::property_tree::ptree const & tree, PointSource& src );
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_POINT_SOURCE_PARSER_HPP_INCLUDED
