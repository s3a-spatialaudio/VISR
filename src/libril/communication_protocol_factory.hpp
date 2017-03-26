/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_COMMUNICATION_PROTOCOL_FACTORY_HPP_INCLUDED
#define VISR_COMMUNICATION_PROTOCOL_FACTORY_HPP_INCLUDED

#include <memory>

#include "export_symbols.hpp"

#include "communication_protocol_base.hpp"
#include "communication_protocol_type.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace visr
{

// Forward declarations
//class CommunicationProtocolType;
class ParameterBase;
class ParameterConfigBase;
enum class ParameterType;

class VISR_CORE_LIBRARY_SYMBOL CommunicationProtocolFactory
{
public:

  static std::unique_ptr<CommunicationProtocolBase> create( CommunicationProtocolType const & protocolType,
                                                            ParameterType const & paramType,
                                                            ParameterConfigBase const & config );

  template< class ConcreteCommunicationProtocol >
  static void registerCommunicationProtocol( CommunicationProtocolType const & protocolType );

private:
  struct Creator
  {
    using CreateFunction = std::function< std::unique_ptr<CommunicationProtocolBase>( ParameterType const &, ParameterConfigBase const & ) >;
    explicit Creator( CreateFunction fcn );

    std::unique_ptr<CommunicationProtocolBase> create( ParameterType const & paramType, ParameterConfigBase const & config ) const;
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

    static std::unique_ptr<CommunicationProtocolBase> construct( ParameterType const & paramType,
                                                                 ParameterConfigBase const & config )
    {
      return std::unique_ptr<CommunicationProtocolBase>( new ConcreteCommunicationProtocolType(paramType, config ));
    }
  };

  using KeyType = CommunicationProtocolType;

  using CreatorTable = std::map< KeyType, Creator >;

  static CreatorTable & creatorTable();
};

template< class ConcreteCommunicationProtocolType >
void CommunicationProtocolFactory::registerCommunicationProtocol( CommunicationProtocolType const & protocolType )
{
  creatorTable( ).insert( std::make_pair( protocolType, TCreator<ConcreteCommunicationProtocolType>() ) );
}

//template< class ConcreteCommunicationProtocolType, CommunicationProtocolType protocolType >
//class CommunicationProtocolRegistrar
//{
//private:
//  /**
//   * Private constructor to prevent instantiation.
//   */
//  CommunicationProtocolRegistrar()
//  {
//    // (void)&sRegistrar;
//  }
//
//  class Registrar
//  {
//  public:
//    Registrar()
//    {
//      CommunicationProtocolFactory::registerCommunicationProtocol<ConcreteCommunicationProtocolType>( protocolType );
//    }
//  };
//
//  static Registrar sRegistrar;
//};
//
//template< class ConcreteCommunicationProtocolType, CommunicationProtocolType protocolType, ParameterType parameterType >
//typename CommunicationProtocolRegistrar<ConcreteCommunicationProtocolType, protocolType, parameterType >::Registrar
//CommunicationProtocolRegistrar<ConcreteCommunicationProtocolType, protocolType, parameterType >::sRegistrar;

} // namespace visr

#endif // #ifndef VISR_COMMUNICATION_PROTOCOL_FACTORY_HPP_INCLUDED
