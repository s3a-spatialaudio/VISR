/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_POLYMORPHIC_PARAMETER_INPUT_HPP_INCLUDED
#define VISR_POLYMORPHIC_PARAMETER_INPUT_HPP_INCLUDED

#include <libril/parameter_input.hpp>
#include <libril/communication_protocol_base.hpp>

#include <memory>

namespace visr
{

class PolymorphicParameterInput: public ParameterInputBase
{
public:
  explicit PolymorphicParameterInput( char const * name,
                               Component & parent,
                               ParameterType const & parameterType,
                               CommunicationProtocolType const & protocolType,
                               ParameterConfigBase const & paramConfig );

  explicit PolymorphicParameterInput( char const * name,
                               Component & parent,
                               ParameterType const & parameterType,
                               CommunicationProtocolType const & protocolType );

  virtual ~PolymorphicParameterInput() override;

  void setProtocol( CommunicationProtocolBase * protocol ) override;

  CommunicationProtocolBase::Input & protocolInput() override;

  CommunicationProtocolBase::Input const & protocolInput() const;

private:
  std::unique_ptr<CommunicationProtocolBase::Input> mProtocolInput;
};

} // namespace visr

#endif // #ifndef VISR_POLYMORPHIC_PARAMETER_INPUT_HPP_INCLUDED
