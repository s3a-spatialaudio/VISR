/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/communication_protocol_base.hpp>

namespace visr
{

CommunicationProtocolBase::CommunicationProtocolBase() = default;

/*virtual*/ CommunicationProtocolBase::~CommunicationProtocolBase() = default;

// Note: The additional documentation is required to suppress Doxygen warnings.

/**
 * @relates CommunicationProtocolBase::Input
 * Default constructor for CommunicationProtocolBase::Input
 */
CommunicationProtocolBase::Input::Input() = default;

/**
 * @relates CommunicationProtocolBase::Input
 * Destructor for CommunicationProtocolBase::Input
 */
/*virtual*/ CommunicationProtocolBase::Input::~Input() = default;

/**
 * @relates CommunicationProtocolBase::Input
 * Default constructor for CommunicationProtocolBase::Output
 */
CommunicationProtocolBase::Output::Output() = default;

/**
 *@relates CommunicationProtocolBase::Input
 * Default constructor for CommunicationProtocolBase::Output
 */
/*virtual*/ CommunicationProtocolBase::Output::~Output() = default;

} // namespace visr

