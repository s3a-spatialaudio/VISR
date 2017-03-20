/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_STATUS_MESSAGE_HPP_INCLUDED
#define VISR_STATUS_MESSAGE_HPP_INCLUDED

namespace visr
{

enum class Status: unsigned char
{
Information, //< An informational message.
Warning,     //< Warning message
Error,
Critical
};

} // namespace visr

#endif // #ifndef VISR_STATUS_MESSAGE_HPP_INCLUDED
