/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_COMMUNICATION_PROTOCOL_TYPE_HPP_INCLUDED
#define VISR_LIBRIL_COMMUNICATION_PROTOCOL_TYPE_HPP_INCLUDED

namespace visr
{
namespace ril
{

enum class CommunicationProtocolType
{
  SharedData,
  DoubleBuffer,
  MessageQueue
  // To be continued.
};

/**
* Metaprogramming construct to translate a type to its corresponding ID.
*/
template< template<typename> class CommunicationProtocolClass, typename ParameterClass >
struct CommunicationProtocolToId {};

template< CommunicationProtocolType CommunicationProtocolTypeId, typename ParameterClass >
struct IdToCommunicationProtocol {};

} // namespace ril
} // namespace visr


#define DEFINE_COMMUNICATION_PROTOCOL_TYPE( CommunicationProtocolClassType, CommunicationProtocolId )\
namespace visr \
{ \
  namespace ril \
  { \
    template< typename ParameterClass > \
    struct CommunicationProtocolToId< CommunicationProtocolClassType, ParameterClass > \
    { \
    public: \
      static const CommunicationProtocolType id = CommunicationProtocolId; \
    }; \
    template<typename ParameterClass > \
    struct IdToCommunicationProtocol<CommunicationProtocolId, ParameterClass> \
    { \
    public: \
      using ConfigType = CommunicationProtocolClassType<ParameterClass>; \
    }; \
  } \
}

#endif // #ifndef VISR_LIBRIL_COMMUNICATION_PROTOCOL_TYPE_HPP_INCLUDED
