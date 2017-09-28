/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_DETAIL_COMPOSE_MESSAGE_STRING_HPP_INCLUDED
#define VISR_DETAIL_COMPOSE_MESSAGE_STRING_HPP_INCLUDED

#include <ostream>
#include <string>
#include <sstream>

namespace visr
{
namespace detail
{
  // Forward declarations
  template< typename MessageType >
  static void composeMessageStream( std::ostream & str, MessageType const & msg );
  template< typename MessageType, typename... MessageRest>
  static void composeMessageStream( std::ostream & str, MessageType const & msg,
                                    MessageRest ... rest );

  /**
   * Create a message string from an arbitrary (non-empty) sequence of arguments.
   * @tparam Messages Parameter pack type containing the arguments.
   * @param messages The pack of parameters.
   * @return String containing the concatenation of the textual representations
   * of the parameters.
   */
  template<typename ... Messages>
  static std::string composeMessageString( Messages ... messages )
  {
    std::stringstream str;
    composeMessageStream( str, messages ... );
    return str.str();
  }

  /**
   * Create a message from an arbitrary sequence of arguments.
   * This is the terminal case of the recursive formatStatusMessage() function above.
   * @tparam MessageType type of the message argument
   * @param str The output stream to which the arguments are written to.
   * @param msg Message argument. All types that have an <<operator() are acceptable.
   */
  template< typename MessageType >
  static void composeMessageStream( std::ostream & str, MessageType const & msg )
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
  static void composeMessageStream( std::ostream & str, MessageType const & msg, MessageRest ... rest )
  {
    str << msg;
    composeMessageStream( str, rest ... );
  }

} // namespace detail
} // namespace visr

#endif // #ifndef VISR_DETAIL_COMPOSE_MESSAGE_STRING_HPP_INCLUDED
