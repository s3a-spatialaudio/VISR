/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_STATUS_MESSAGE_HPP_INCLUDED
#define VISR_STATUS_MESSAGE_HPP_INCLUDED

#include <ostream>

namespace visr
{

/**
 * Class encapsulating facilities to signal messages and errors.
 */
class StatusMessage
{
public:
  /**
   * Enumeration for different categories of status message.
   */
  enum Kind: unsigned char
  {
    Information, /*!<  An informational message. */
    Warning,     /*!< Warning message */
    Error,       /*!< Error message, the issuer is responsible for
                  * returning from the current callback call. The calling class
                  * (i.e., a component will be called again in the nest iteration. */
    Abort,       /*!< Severe error. Execution should be terminated after the current iteration */
    Critical     /*!< Critical error, control shall not return to the calling function.*/
  };

  /**
  * Create a status message from an arbitrary sequence of arguments.
  * This template function is invoked recursively (at compile time)
  * @tparam MessageType type of the first argument
  * @tparam MessageRest parameter type holding all arguments apart from the rest.
  * @param str The output stream to which the arguments are written to.
  * @param msg First argument. All types that have an <<operator() are acceptable.
  * @param rest variable-length list containing all parameters except the first.
  */
  template< typename MessageType, typename... MessageRest>
  static void format( std::ostream & str, MessageType const & msg, MessageRest ... rest )
  {
    str << msg;
    format( str, rest ... );
  }

  /**
  * Create a status message from an arbitrary sequence of arguments.
  * This is the terminal case of the recursive formatStatusMessage() function above.
  * @tparam MessageType type of the message argument
  * @param str The output stream to which the arguments are written to.
  * @param msg Message argument. All types that have an <<operator() are acceptable.
  */
  template< typename MessageType >
  static void format( std::ostream & str, MessageType const & msg )
  {
    str << msg;
  }
};

} // namespace visr

#endif // #ifndef VISR_STATUS_MESSAGE_HPP_INCLUDED
