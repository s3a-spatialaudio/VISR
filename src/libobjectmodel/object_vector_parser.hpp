/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_OBJECT_VECTOR_PARSER_HPP_INCLUDED
#define VISR_OBJECTMODEL_OBJECT_VECTOR_PARSER_HPP_INCLUDED

#include <iosfwd>
#include <string>

namespace visr
{
namespace objectmodel
{
// forward declaration
class ObjectVector;


class ObjectVectorParser
{
public:
  static void fillObjectVector( std::string const & message, ObjectVector & res );
  static void fillObjectVector( std::basic_istream<char> & message, ObjectVector & res );

  static void fillObjectVector( char const * message, ObjectVector & res );

  static void updateObjectVector( std::string const & message, ObjectVector & res );
  static void updateObjectVector( std::basic_istream<char> & message, ObjectVector & res );

  static void updateObjectVector( char const * message, ObjectVector & res );
};

} // namespace objectmodel
} // namespace visr

#endif // #ifndef VISR_OBJECTMODEL_OBJECT_VECTOR_PARSER_HPP_INCLUDED
