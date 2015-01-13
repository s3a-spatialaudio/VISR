/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_OBJECT_PARSER_HPP_INCLUDED
#define VISR_OBJECTMODEL_OBJECT_PARSER_HPP_INCLUDED

#include "object.hpp"

#include <boost/property_tree/ptree_fwd.hpp>

namespace visr
{
namespace objectmodel
{

/**
 * @todo revise class hierarchy (does it make sense to let the base of the parser object
 * hierarchy to perform actual work, or should there be an abstract interface class on top of it?
 */
class ObjectParser
{
public:
  /**
   * @note: This function is marked as pure virtual, thus making the class abstract 
   * (non-instantiable). However, it has an implementation which performs sensible work.
   */
  virtual void parse( boost::property_tree::ptree const & tree, Object & obj ) const = 0;

protected:

private:
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_OBJECT_PARSER_HPP_INCLUDED
