/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_POLYMORPHIC_PARAMETER_INPUT_HPP_INCLUDED
#define VISR_POLYMORPHIC_PARAMETER_INPUT_HPP_INCLUDED

#include <libril/export_symbols.hpp>
#include <libril/parameter_input.hpp>
#include <libril/communication_protocol_base.hpp>

#include <memory>

namespace visr
{

class PolymorphicParameterInput: public ParameterInputBase
{
public:
  VISR_CORE_LIBRARY_SYMBOL explicit PolymorphicParameterInput( char const * name,
                               Component & parent,
                               ParameterType const & parameterType,
                               CommunicationProtocolType const & protocolType,
                               ParameterConfigBase const & paramConfig );

  VISR_CORE_LIBRARY_SYMBOL explicit PolymorphicParameterInput( char const * name,
                               Component & parent,
                               ParameterType const & parameterType,
                               CommunicationProtocolType const & protocolType );

  VISR_CORE_LIBRARY_SYMBOL virtual ~PolymorphicParameterInput() override;

  VISR_CORE_LIBRARY_SYMBOL void setProtocol( CommunicationProtocolBase * protocol ) override;

  VISR_CORE_LIBRARY_SYMBOL CommunicationProtocolBase::Input & protocolInput() override;

  VISR_CORE_LIBRARY_SYMBOL CommunicationProtocolBase::Input const & protocolInput() const;

private:
  std::unique_ptr<CommunicationProtocolBase::Input> mProtocolInput;
};

} // namespace visr

#endif // #ifndef VISR_POLYMORPHIC_PARAMETER_INPUT_HPP_INCLUDED
