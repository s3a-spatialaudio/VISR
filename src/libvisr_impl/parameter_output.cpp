/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libvisr/parameter_output.hpp>
#include <libvisr/port_base.hpp> // for PortBase::Direction

namespace visr
{

ParameterOutputBase::ParameterOutputBase( char const * name,
                                        Component & parent,
                                        ParameterType const & parameterType,
                                        CommunicationProtocolType const & protocolType,
                                        ParameterConfigBase const & paramConfig )
  : ParameterPortBase( name, parent, PortBase::Direction::Output, parameterType, protocolType, paramConfig )
{}

ParameterOutputBase::ParameterOutputBase( char const * name,
    Component & parent,
    ParameterType const & parameterType,
    CommunicationProtocolType const & protocolType )
  : ParameterPortBase( name, parent, PortBase::Direction::Output, parameterType, protocolType )
{}

ParameterOutputBase::~ParameterOutputBase() = default;

} // namespace visr
