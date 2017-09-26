/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libvisr/parameter_input.hpp>

namespace visr
{

ParameterInputBase::ParameterInputBase( char const * name,
                                        Component & parent,
                                        ParameterType const & parameterType,
                                        CommunicationProtocolType const & protocolType,
                                        ParameterConfigBase const & paramConfig )
  : ParameterPortBase( name, parent, PortBase::Direction::Input, parameterType, protocolType, paramConfig )
{}

ParameterInputBase::ParameterInputBase( char const * name,
    Component & parent,
    ParameterType const & parameterType,
    CommunicationProtocolType const & protocolType )
  : ParameterPortBase( name, parent, PortBase::Direction::Input, parameterType, protocolType )
{}

ParameterInputBase::~ParameterInputBase() = default;

} // namespace visr
