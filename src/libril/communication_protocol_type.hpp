/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_COMMUNICATION_PROTOCOL_TYPE_HPP_INCLUDED
#define VISR_COMMUNICATION_PROTOCOL_TYPE_HPP_INCLUDED

#include "detail/compile_time_hash_fnv1.hpp"

#include <string>
#include <cstddef>

namespace visr
{

using CommunicationProtocolType = std::uint64_t;

constexpr CommunicationProtocolType communicationProtocolTypeFromString( char const * typeString )
{
  return detail::compileTimeHashFNV1( typeString );
}


/**
* Metaprogramming construct to translate a type to its corresponding ID.
*/
template<class CommunicationProtocolClass >
struct CommunicationProtocolToId {};

template< CommunicationProtocolType id>
struct IdToCommunicationProtocol {};

} // namespace visr


#define DEFINE_COMMUNICATION_PROTOCOL( CommunicationProtocolClassType, CommunicationProtocolId, CommunicationProtocolName )\
namespace visr \
{ \
   template<>\
   struct CommunicationProtocolToId< CommunicationProtocolClassType > \
    { \
    public: \
      enum : CommunicationProtocolType { id = CommunicationProtocolId };\
      static constexpr const char * name = CommunicationProtocolName; \
    }; \
}

#endif // #ifndef VISR_COMMUNICATION_PROTOCOL_TYPE_HPP_INCLUDED
