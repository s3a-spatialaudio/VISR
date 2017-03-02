/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_COMMUNICATION_PROTOCOL_FACTORY_HPP_INCLUDED
#define VISR_COMMUNICATION_PROTOCOL_FACTORY_HPP_INCLUDED

#include "export_symbols.hpp"

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

// Forward declarations
// class CommunicationProtocolBase;
// class CommunicationProtocolType;
class ParameterBase;
class ParameterConfigBase;

/*VISR_CORE_LIBRARY_SYMBOL*/ // TODO: Get the library symbol exporting right later on.
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
  std::pair<CommunicationProtocolType, ParameterType> const key = std::make_pair( protocolType, paramType );
  creatorTable( ).insert( std::make_pair( key, TCreator<ConcreteCommunicationProtocolType>() ) );
}

template< class ConcreteCommunicationProtocolType, CommunicationProtocolType protocolType, ParameterType parameterType >
class CommunicationProtocolRegistrar
{
private:
  /**
   * Private constructor to prevent instantiation.
   */
  CommunicationProtocolRegistrar()
  {
    (void)&sRegistrar;
  }

  class Registrar
  {
  public:
    Registrar()
    {
      CommunicationProtocolFactory::registerCommunicationProtocolType<ConcreteCommunicationProtocolType>( protocolType, parameterType );
    }
  };

  static Registrar sRegistrar;
};

template< class ConcreteCommunicationProtocolType, CommunicationProtocolType protocolType, ParameterType parameterType >
typename CommunicationProtocolRegistrar<ConcreteCommunicationProtocolType, protocolType, parameterType >::Registrar
CommunicationProtocolRegistrar<ConcreteCommunicationProtocolType, protocolType, parameterType >::sRegistrar;

} // namespace visr

#endif // #ifndef VISR_COMMUNICATION_PROTOCOL_FACTORY_HPP_INCLUDED
