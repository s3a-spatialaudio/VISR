/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_COMMUNICATION_PROTOCOL_TYPE_HPP_INCLUDED
#define VISR_LIBRIL_COMMUNICATION_PROTOCOL_TYPE_HPP_INCLUDED

namespace visr
{
namespace ril
{

enum class CommunicationProtocolType
{
  SharedMemory,
  DoubleBuffer,
  MessageQueue
  // To be continued.
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_COMMUNICATION_PROTOCOL_TYPE_HPP_INCLUDED
