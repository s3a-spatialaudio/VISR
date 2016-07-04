/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_COMMUNICATION_PROTOCOL_FACTORY_HPP_INCLUDED
#define VISR_LIBRIL_COMMUNICATION_PROTOCOL_FACTORY_HPP_INCLUDED

#include "parameter_type.hpp"

#include "communication_protocol_base.hpp"
#include "communication_protocol_type.hpp"

// HACK
// For the moment, include the communication protocols and parameter types here.
// Later, move the registration to libraries (libpml)
#include <libpml/message_queue.hpp>
#include <libpml/matrix_parameter.hpp>
#include <libpml/string_parameter.hpp>

#include <map>
#include <memory>
#include <string>

#include <boost/function.hpp>

namespace visr
{
namespace ril
{

// Forward declarations
// class CommunicationProtocolBase;
// class CommunicationProtocolType;
class ParameterBase;
class ParameterConfigBase;

class CommunicationProtocolFactory
{
public:
  static std::unique_ptr<CommunicationProtocolBase> create( CommunicationProtocolType const & protocolType,
                                                            ParameterType const & paramType,
                                                            ParameterConfigBase const & config );

  template< class ConcreteCommunicationProtocol >
  static void registerCommunicationProtocolType( CommunicationProtocolType const & protocolType,
                                                 ParameterType const & paramType );

private:
  struct Creator
  {
    using CreateFunction = boost::function< CommunicationProtocolBase* ( ParameterConfigBase const & config ) >;

    explicit Creator( CreateFunction fcn );

    std::unique_ptr<CommunicationProtocolBase> create( ParameterConfigBase const & config ) const;
 private:
    CreateFunction mCreateFunction;
  };

  template< class ConcreteCommunicationProtocolType >
  class TCreator: public Creator
  {
  public:
    TCreator( )
      : Creator( &TCreator<ConcreteCommunicationProtocolType>::construct )
    {
    }

    static CommunicationProtocolBase* construct( ParameterConfigBase const & config )
    {
      CommunicationProtocolBase* obj = new ConcreteCommunicationProtocolType( config );
      return obj;
    }
  };

  using KeyType = std::pair<CommunicationProtocolType, ParameterType >;

  using CreatorTable = std::map< KeyType, Creator >;

  static CreatorTable & creatorTable();
};

template< class ConcreteCommunicationProtocolType >
void CommunicationProtocolFactory::registerCommunicationProtocolType( CommunicationProtocolType const & protocolType,
                                                                      ParameterType const & paramType )
{
  // using ParameterClass = pml::MatrixParameter<float>; // IdToParameter<paramType>::Type;
  
  std::pair<CommunicationProtocolType, ParameterType> const key = std::make_pair( protocolType, paramType );
  creatorTable( ).insert( std::make_pair( key, TCreator<ConcreteCommunicationProtocolType>() ) );
}

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_COMMUNICATION_PROTOCOL_FACTORY_HPP_INCLUDED
