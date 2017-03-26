/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_COMMUNICATION_PROTOCOL_TYPE_HPP_INCLUDED
#define VISR_COMMUNICATION_PROTOCOL_TYPE_HPP_INCLUDED

#include "detail/compile_time_hash_fnv1.hpp"

#include <string>
#include <cstddef>

namespace visr
{

//namespace detail
//{
//  // Non-cryptographic compile-time hash function.
//  // Adapted from https://github.com/elbeno/constexpr
//  // Licence: MIT
//  constexpr uint64_t fnv1( uint64_t h, const char* s )
//  {
//    return (*s == 0) ? h :
//      fnv1( static_cast<uint64_t>(h * 1099511628211ull) ^
//          static_cast<uint64_t>(*s), s + 1 );
//  }
//
//  constexpr uint64_t fnv1( const char* s )
//  {
//    return true ?
//      detail::fnv1( 14695981039346656037ull, s ) :
//      throw std::logic_error("FNV1 hash failed.");
//  }
//}

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


#define DEFINE_COMMUNICATION_PROTOCOL( CommunicationProtocolClassType, CommunicationProtocolId )\
namespace visr \
{ \
   template<>\
   struct CommunicationProtocolToId< CommunicationProtocolClassType > \
    { \
    public: \
      static const CommunicationProtocolType id = CommunicationProtocolId; \
    }; \
}

//template<>\
//struct IdToCommunicationProtocol<CommunicationProtocolId> \
//{ \
//public: \
//  using ConfigType = CommunicationProtocolClassType; \
//}; \

#endif // #ifndef VISR_COMMUNICATION_PROTOCOL_TYPE_HPP_INCLUDED
