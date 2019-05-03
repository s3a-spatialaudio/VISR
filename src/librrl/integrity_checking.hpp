/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_INTEGRITY_CHECKING_HPP_INCLUDED
#define VISR_LIBRRL_INTEGRITY_CHECKING_HPP_INCLUDED

/**
 * @file librrl/integrity_checking.hpp
 * Checking functions for signal flow components.
 * Functions are provided for checking the audio connections, parameter connections, or both.
 * Functions check either only the current level of a composite signal flow (option hierarchical=false), 
 * or all contained components recursively (option hierarchical=true).
 */

#include "export_symbols.hpp"

#include <iosfwd>

namespace visr
{
// Forward declarations
class Component;
namespace impl
{
class ComponentImplementation;
}

namespace rrl
{

/**
 * Check both audio and parameter connections for validity and completeness.
 * This is a convenience function that accepts impl::Component objects.
 * @param component The signal flow component to check.
 * @param hierarchical Whether only the component is checked locally (value: false) or whether all contained sub-components are also checked recursively ((true)
 * @param[out] messages A stream object returning explanatory and diagnostic messages.
 * @return Whether the component's connections are valid.
 */
VISR_RRL_LIBRARY_SYMBOL bool checkConnectionIntegrity( Component const & component,
						       bool hierarchical,
						       std::ostream & messages );

/**
 * Check the audio connections of a component  for validity and completeness.
 * This is a convenience function that accepts impl::Component objects.
 * @param component The signal flow component to check.
 * @param hierarchical Whether only the component is checked locally (value: false) or whether all contained sub-components are also checked recursively ((true)
 * @param[out] messages A stream object returning explanatory and diagnostic messages.
 * @return Whether the component's audio connections are valid.
 */
VISR_RRL_LIBRARY_SYMBOL bool checkAudioConnectionIntegrity( Component const & component,
							    bool hierarchical,
							    std::ostream & messages );

/**
 * Check the parameter connections of a component for validity and completeness.
 * This is a convenience function that accepts impl::Component objects.
 * @param component The signal flow component to check.
 * @param hierarchical Whether only the component is checked locally (value: false) or whether all contained sub-components are also checked recursively ((true)
 * @param[out] messages A stream object returning explanatory and diagnostic messages.
 * @return Whether the component's parameter connections are valid.
 */
bool checkParameterConnectionIntegrity( Component const & component, bool hierarchical, std::ostream & messages );

/**
 * Check both audio and parameter connections  for validity and completeness.
 * @param component The signal flow component to check.
 * @param hierarchical Whether only the component is checked locally (value: false) or whether all contained sub-components are also checked recursively ((true)
 * @param[out] messages A stream object returning explanatory and diagnostic messages.
 * @return Whether the component's connections are valid.
 */
VISR_RRL_LIBRARY_SYMBOL bool checkConnectionIntegrity( impl::ComponentImplementation const & component,
						       bool hierarchical,
						       std::ostream & messages );

/**
 * Check the audio connections of a component  for validity and completeness.
 * @param component The signal flow component to check.
 * @param hierarchical Whether only the component is checked locally (value: false) or whether all contained sub-components are also checked recursively ((true)
 * @param[out] messages A stream object returning explanatory and diagnostic messages.
 * @return Whether the component's audio connections are valid.
 */
VISR_RRL_LIBRARY_SYMBOL bool checkAudioConnectionIntegrity( impl::ComponentImplementation const & component,
							    bool hierarchical,
							    std::ostream & messages );

/**
 * Check the parameter connections of a component  for validity and completeness.
 * This is a convenience function that accepts impl::Component objects.
 * @param component The signal flow component to check.
 * @param hierarchical Whether only the component is checked locally (value: false) or whether all contained sub-components are also checked recursively ((true)
 * @param[out] messages A stream object returning explanatory and diagnostic messages.
 * @return Whether the component's parameter connections are valid.
 */
VISR_RRL_LIBRARY_SYMBOL bool checkParameterConnectionIntegrity( impl::ComponentImplementation const & component,
								bool hierarchical,
								std::ostream & messages );

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_INTEGRITY_CHECKING_HPP_INCLUDED
