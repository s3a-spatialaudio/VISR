/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_COMMUNICATION_PROTOCOL_BASE_HPP_INCLUDED
#define VISR_LIBRIL_COMMUNICATION_PROTOCOL_BASE_HPP_INCLUDED

#include "parameter_type.hpp"

namespace visr
{
namespace ril
{

/**
 *
 *
 */
class CommunicationProtocolBase
{
public:

  CommunicationProtocolBase();

  /**
   *
   */
  virtual ~CommunicationProtocolBase();

  virtual ParameterType type() const = 0;

};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_COMMUNICATION_PROTOCOL_BASE_HPP_INCLUDED
