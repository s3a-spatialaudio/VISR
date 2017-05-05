/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_CONNECTION_DESCRIPTOR_HPP_INCLUDED
#define VISR_PARAMETER_CONNECTION_DESCRIPTOR_HPP_INCLUDED

#include <set>

namespace visr
{

namespace impl
{
class ParameterPortBaseImplementation;

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

  ParameterConnection( ParameterPortBaseImplementation * pSender,
                       ParameterPortBaseImplementation * pReceiver );

  bool operator<( ParameterConnection const & rhs ) const;

  ParameterPortBaseImplementation * sender() const { return mSender; }
  ParameterPortBaseImplementation * receiver() const { return mReceiver; }


private:
  ParameterPortBaseImplementation * mSender;
  ParameterPortBaseImplementation * mReceiver;
};

using ParameterConnectionTable = std::multiset<ParameterConnection>;

} // namespace impl
} // namespace visr

#endif // #ifndef VISR_PARAMETER_CONNECTION_DESCRIPTOR_HPP_INCLUDED
