/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "polymorphic_parameter_output.hpp"

#include <libril/communication_protocol_factory.hpp>

namespace visr
{

PolymorphicParameterOutput::PolymorphicParameterOutput( char const * name,
                                        Component & parent,
                                        ParameterType const & parameterType,
                                        CommunicationProtocolType const & protocolType,
                                        ParameterConfigBase const & paramConfig )
  : ParameterOutputBase( name, parent, parameterType, protocolType, paramConfig )
{}

PolymorphicParameterOutput::PolymorphicParameterOutput( char const * name,
    Component & parent,
    ParameterType const & parameterType,
    CommunicationProtocolType const & protocolType )
  : ParameterOutputBase( name, parent, parameterType, protocolType )
  , mProtocolOutput( nullptr )
{}

PolymorphicParameterOutput::~PolymorphicParameterOutput() = default;

void PolymorphicParameterOutput::setProtocol( CommunicationProtocolBase * protocol )
{
  mProtocolOutput->setProtocolInstance( protocol );
}

CommunicationProtocolBase::Output & 
PolymorphicParameterOutput::protocolOutput()
{
  return *mProtocolOutput;
}

CommunicationProtocolBase::Output const &
PolymorphicParameterOutput::protocolOutput() const
{
  return *mProtocolOutput;
}


} // namespace visr
