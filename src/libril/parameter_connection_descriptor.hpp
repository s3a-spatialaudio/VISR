/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_PARAMETER_CONNECTION_DESCRIPTORHPP_INCLUDED
#define VISR_LIBRIL_PARAMETER_CONNECTION_DESCRIPTORHPP_INCLUDED


#include <array>
#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <stdexcept>
#include <sstream>
#include <valarray>

namespace visr
{
namespace ril
{

struct ParameterPortDescriptor
{
public:
  ParameterPortDescriptor() = default;

  explicit ParameterPortDescriptor( std::string const & pComponent, std::string const & pPort );

  bool operator<(ParameterPortDescriptor const & rhs) const;

  std::string const & component() const { return mComponent; }
  std::string const & port() const { return mPort; }

private:
  std::string mComponent;
  std::string mPort;
};

// Not used in the current code.
#if 0
/**
 * Store data from definition in derived class until initialisation of runtime structures.
 * @todo This should move into CompositeComponent.
 */
struct ParameterConnection
{
public:
  /**
   * Default constructor, required for use in standard containers.
   * Creates a struct with empty strings for all members.
   */
  ParameterConnection() = default;

  ParameterConnection( ParameterPortDescriptor const & pSender,
                     ParameterPortDescriptor const & pReceiver );

  ParameterConnection( std::string const & pSendComponent,
                       std::string const & pSendPort,
                       std::string const & pReceiveComponent,
                       std::string const & pReceivePort );

  bool operator<(ParameterConnection const & rhs) const;

  ParameterPortDescriptor const & sender() const { return mSender; }
  ParameterPortDescriptor const & receiver() const { return mReceiver; }
private:
  ParameterPortDescriptor const mSender;
  ParameterPortDescriptor const mReceiver;
};
#endif

using ParameterConnectionTable = std::multimap<ParameterPortDescriptor, ParameterPortDescriptor >;

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_PARAMETER_CONNECTION_DESCRIPTORHPP_INCLUDED
