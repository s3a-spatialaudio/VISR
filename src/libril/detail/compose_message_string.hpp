/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_DETAIL_COMPOSE_MESSAGE_STRING_HPP_INCLUDED
#define VISR_DETAIL_COMPOSE_MESSAGE_STRING_HPP_INCLUDED

#include <string>
#include <ostream>

namespace visr
{
namespace detail
{

  /**
  * Create a message from an arbitrary sequence of arguments.
  * This is the terminal case of the recursive formatStatusMessage() function above.
  * @tparam MessageType type of the message argument
  * @param str The output stream to which the arguments are written to.
  * @param msg Message argument. All types that have an <<operator() are acceptable.
  */
  template< typename MessageType >
  static void composeMessageString( std::ostream & str, MessageType const & msg )
  {
    str << msg;
  }


  /**
  * Create a message from an arbitrary sequence of arguments.
  * This template function is invoked recursively (at compile time)
  * @tparam MessageType type of the first argument
  * @tparam MessageRest parameter type holding all arguments apart from the rest.
  * @param str The output stream to which the arguments are written to.
  * @param msg First argument. All types that have an <<operator() are acceptable.
  * @param rest variable-length list containing all parameters except the first.
  */
  template< typename MessageType, typename... MessageRest>
  static void composeMessageString( std::ostream & str, MessageType const & msg, MessageRest ... rest )
  {
    str << msg;
    composeMessageString( str, rest ... );
  }

} // namespace detail
} // namespace visr

#endif // #ifndef VISR_DETAIL_COMPOSE_MESSAGE_STRING_HPP_INCLUDED
