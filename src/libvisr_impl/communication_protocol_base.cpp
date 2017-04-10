/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/communication_protocol_base.hpp>

namespace visr
{
/*virtual*/ CommunicationProtocolBase::~CommunicationProtocolBase() = default;

/*virtual*/ CommunicationProtocolBase::Input::~Input() = default;

/*virtual*/ CommunicationProtocolBase::Output::~Output() = default;

} // namespace visr

