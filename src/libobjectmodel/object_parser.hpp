/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_OBJECT_PARSER_HPP_INCLUDED
#define VISR_OBJECTMODEL_OBJECT_PARSER_HPP_INCLUDED

#include "object.hpp"

#include "export_symbols.hpp"

#include <boost/property_tree/ptree_fwd.hpp>

namespace visr
{
namespace objectmodel
{

/**
 * @todo revise class hierarchy (does it make sense to let the base of the parser object
 * hierarchy to perform actual work, or should there be an abstract interface class on top of it?
 */
class VISR_OBJECTMODEL_LIBRARY_SYMBOL ObjectParser
{
public:
  /**
   * Parse an object from a property node.
   * @param tree The property tree node containing the object information.
   * @param[out] obj The object to which the parsed values are set.
   * @note: This function is marked as pure virtual, thus making the class abstract 
   * (non-instantiable). However, it has an implementation which performs sensible work.
   */
  virtual void parse( boost::property_tree::ptree const & tree, Object & obj ) const = 0;

  /**
   * Serialize an object itno a property tree node.
   * @param obj The audio object to be serialised.
   * @param[out] tree The property tree subtree to where the object information is written to.
   * @note This method is pure virtual, because his class does not correspond to an instantiable audio object type.
   * Nonetheless, it has an implementation which is called by derived classes.
   */
  virtual void write( Object const & obj, boost::property_tree::ptree & tree ) const = 0;
protected:

private:
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_OBJECT_PARSER_HPP_INCLUDED
