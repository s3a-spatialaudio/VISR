/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_OBJECT_VECTOR_PARSER_HPP_INCLUDED
#define VISR_OBJECTMODEL_OBJECT_VECTOR_PARSER_HPP_INCLUDED

#include "export_symbols.hpp"

#include <boost/property_tree/ptree_fwd.hpp>

#include <iosfwd>
#include <memory>
#include <string>

namespace visr
{
namespace objectmodel
{
// forward declaration
class ObjectVector;


class VISR_OBJECTMODEL_LIBRARY_SYMBOL ObjectVectorParser
{
public:
  static void fillObjectVector( std::string const & message, ObjectVector & res );
  static void fillObjectVector( std::basic_istream<char> & message, ObjectVector & res );

  static void fillObjectVector( char const * message, ObjectVector & res );

  static void updateObjectVector( std::string const & message, ObjectVector & res );
  static void updateObjectVector( std::basic_istream<char> & message, ObjectVector & res );

  static void updateObjectVector( char const * message, ObjectVector & res );

  static void encodeObjectVector( ObjectVector const & objects,
                                  std::basic_ostream<char> & message );
private:

  /**
   * Fill an ObjectVector data structure from a property tree data structure.
   * @param subtree Property tree representing a JSON object.
   * @param [out] res The resulting object vector. If the variable contained object before the call, they are cleared.
   * @throw std::exception if a 
   */
  static void parseObject( boost::property_tree::ptree const & subtree, ObjectVector & res );
};

} // namespace objectmodel
} // namespace visr

#endif // #ifndef VISR_OBJECTMODEL_OBJECT_VECTOR_PARSER_HPP_INCLUDED
