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
  static void registerCommunicationProtocol( CommunicationProtocolType const & protocolType, char const * name );

  template< class ConcreteCommunicationProtocol >
  static void registerCommunicationProtocol();

private:
  struct Creator
  {
    using CreateFunction = std::function< std::unique_ptr<CommunicationProtocolBase>( ParameterType const &, ParameterConfigBase const & ) >;
    explicit Creator( CreateFunction fcn, char const * name );

    std::unique_ptr<CommunicationProtocolBase> create( ParameterType const & paramType, ParameterConfigBase const & config ) const;

    std::string const & name() const;
 private:
    CreateFunction mCreateFunction;
    std::string mName;
  };

  template< class ConcreteCommunicationProtocolType >
  class TCreator: public Creator
  {
  public:
    TCreator( char const * name)
      : Creator( &TCreator<ConcreteCommunicationProtocolType>::construct, name )
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
void CommunicationProtocolFactory::registerCommunicationProtocol( CommunicationProtocolType const & protocolType, char const * name )
{
  creatorTable( ).insert( std::make_pair( protocolType, TCreator<ConcreteCommunicationProtocolType>(name) ) );
}

template< class ConcreteCommunicationProtocolType >
void CommunicationProtocolFactory::registerCommunicationProtocol()
{
  registerCommunicationProtocol<ConcreteCommunicationProtocolType>( 
    CommunicationProtocolToId<ConcreteCommunicationProtocolType>::id, 
    CommunicationProtocolToId<ConcreteCommunicationProtocolType>::name ) ;
}


} // namespace visr

#endif // #ifndef VISR_COMMUNICATION_PROTOCOL_FACTORY_HPP_INCLUDED
