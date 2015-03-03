/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_PLANE_WAVE_PARSER_HPP_INCLUDED
#define VISR_OBJECTMODEL_PLANE_WAVE_PARSER_HPP_INCLUDED

#include "object_parser.hpp"

#include "object.hpp"

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class PlaneWaveParser: public ObjectParser
{
public:
  virtual void parse( boost::property_tree::ptree const & tree, Object & src ) const;

  virtual void write( Object const & obj, boost::property_tree::ptree & tree ) const;
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_PLANE_WAVE_PARSER_HPP_INCLUDED
