/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "communication_protocol_base.hpp"

namespace visr
{

CommunicationProtocolBase::CommunicationProtocolBase() = default;

/*virtual*/ CommunicationProtocolBase::~CommunicationProtocolBase() = default;

// Note: This code needs to be excluded from Doxygen documentation generation to avoid warnings
// caused by shared symbol visibility macros in the class definitions.
/// @cond NEVER
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
 * @relates CommunicationProtocolBase::Output
 * Default constructor for CommunicationProtocolBase::Output
 */
CommunicationProtocolBase::Output::Output() = default;

/**
 *@relates CommunicationProtocolBase::Output
 * Default constructor for CommunicationProtocolBase::Output
 */
/*virtual*/ CommunicationProtocolBase::Output::~Output() = default;
/// @endcond NEVER

} // namespace visr

