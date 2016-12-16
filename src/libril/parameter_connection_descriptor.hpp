/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_PARAMETER_CONNECTION_DESCRIPTOR_HPP_INCLUDED
#define VISR_LIBRIL_PARAMETER_CONNECTION_DESCRIPTOR_HPP_INCLUDED

#include <set>

namespace visr
{
namespace ril
{
class ParameterPortBase;

#if 0
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
#endif

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
  ParameterConnection()
    : mSender( nullptr )
    , mReceiver( nullptr )
  {
  }

  ParameterConnection( ParameterPortBase * pSender,
                       ParameterPortBase * pReceiver );

  bool operator<( ParameterConnection const & rhs ) const;

  ParameterPortBase * sender() const { return mSender; }
  ParameterPortBase * receiver() const { return mReceiver; }


private:
  ParameterPortBase * mSender;
  ParameterPortBase * mReceiver;
};

// using ParameterConnectionTable = std::multimap<ParameterPortDescriptor, ParameterPortDescriptor >;
using ParameterConnectionTable = std::multiset<ParameterConnection>;

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_PARAMETER_CONNECTION_DESCRIPTOR_HPP_INCLUDED
