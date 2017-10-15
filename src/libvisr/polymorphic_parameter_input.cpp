/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "polymorphic_parameter_input.hpp"

#include "communication_protocol_factory.hpp"

namespace visr
{

PolymorphicParameterInput::PolymorphicParameterInput( char const * name,
                                        Component & parent,
                                        ParameterType const & parameterType,
                                        CommunicationProtocolType const & protocolType,
                                        ParameterConfigBase const & paramConfig )
  : ParameterInputBase( name, parent, parameterType, protocolType, paramConfig )
  , mProtocolInput( CommunicationProtocolFactory::createInput( protocolType )  )
{}

PolymorphicParameterInput::PolymorphicParameterInput( char const * name,
    Component & parent,
    ParameterType const & parameterType,
    CommunicationProtocolType const & protocolType )
  : ParameterInputBase( name, parent, parameterType, protocolType )
  , mProtocolInput( CommunicationProtocolFactory::createInput( protocolType ) )
{}

PolymorphicParameterInput::~PolymorphicParameterInput() = default;

void PolymorphicParameterInput::setProtocol( CommunicationProtocolBase * protocol )
{
  mProtocolInput->setProtocolInstance( protocol );
}

CommunicationProtocolBase::Input & 
PolymorphicParameterInput::protocolInput()
{
  return *mProtocolInput;
}

CommunicationProtocolBase::Input const &
PolymorphicParameterInput::protocolInput() const
{
  return *mProtocolInput;
}


} // namespace visr
