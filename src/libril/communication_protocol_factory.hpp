/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_COMMUNICATION_PROTOCOL_FACTORY_HPP_INCLUDED
#define VISR_COMMUNICATION_PROTOCOL_FACTORY_HPP_INCLUDED

#include <memory>

#include "communication_protocol_base.hpp"
#include "communication_protocol_type.hpp"
#include "export_symbols.hpp"
#include "parameter_type.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace visr
{

// Forward declarations
class ParameterBase;
class ParameterConfigBase;

class CommunicationProtocolFactory
{
public:

  VISR_CORE_LIBRARY_SYMBOL static std::unique_ptr<CommunicationProtocolBase> create( CommunicationProtocolType const & protocolType,
                                                            ParameterType const & paramType,
                                                            ParameterConfigBase const & config );

  VISR_CORE_LIBRARY_SYMBOL static std::unique_ptr<CommunicationProtocolBase::Input> createInput( CommunicationProtocolType const & protocolType );
  VISR_CORE_LIBRARY_SYMBOL static std::unique_ptr<CommunicationProtocolBase::Output> createOutput( CommunicationProtocolType const & protocolType );

  /**
   * Return the number of registered protocols.
   */
  VISR_CORE_LIBRARY_SYMBOL static std::size_t numberOfProtocols() noexcept;

  /**
   * Return the registered name of a protocol.
   * @throw std::invalid_argument if no protocol with this name exists.
   */
  VISR_CORE_LIBRARY_SYMBOL static CommunicationProtocolType typeFromName( char const * name );

  /**
   * Lookup the protocol type for a name.
   * @throw std::invalid_argument if no protocol with this name has
   * been registered.
   * @todo Consider changing the return type to char const *
   */
  VISR_CORE_LIBRARY_SYMBOL static std::string typeToName( CommunicationProtocolType type );

  VISR_CORE_LIBRARY_SYMBOL static bool typeExists( CommunicationProtocolType type ) noexcept;


  template< class ConcreteCommunicationProtocol >
  static void registerCommunicationProtocol( CommunicationProtocolType const & protocolType, char const * name );

  template< class ConcreteCommunicationProtocol >
  static void registerCommunicationProtocol();

private:
  struct Creator
  {
    using CreateFunction = std::function< std::unique_ptr<CommunicationProtocolBase>( ParameterType const &, ParameterConfigBase const & ) >;
    using InputCreateFunction = std::function< std::unique_ptr<CommunicationProtocolBase::Input>() >;
    using OutputCreateFunction = std::function< std::unique_ptr<CommunicationProtocolBase::Output>() >;
    VISR_CORE_LIBRARY_SYMBOL explicit Creator( CreateFunction fcn,
                      InputCreateFunction inputCreator,
                      OutputCreateFunction outputCreator,
                      char const * name );

    VISR_CORE_LIBRARY_SYMBOL std::unique_ptr<CommunicationProtocolBase> create( ParameterType const & paramType, ParameterConfigBase const & config ) const;
    VISR_CORE_LIBRARY_SYMBOL std::unique_ptr<CommunicationProtocolBase::Input> createInput() const;
    VISR_CORE_LIBRARY_SYMBOL std::unique_ptr<CommunicationProtocolBase::Output> createOutput() const;

    VISR_CORE_LIBRARY_SYMBOL std::string const & name() const;
 private:

    CreateFunction mCreateFunction;
    InputCreateFunction mInputCreateFunction;
    OutputCreateFunction mOutputCreateFunction;
    std::string mName;
  };

  /**
   * Internal registration function.
   * @throw std::invalid_argument If a creator is already registered for the given protocol type \p type
   */
  VISR_CORE_LIBRARY_SYMBOL static void registerCommunicationProtocol( CommunicationProtocolType type, Creator&& creator );

  template< class ConcreteCommunicationProtocolType >
  class TCreator: public Creator
  {
  public:
    TCreator( char const * name)
      : Creator( &TCreator<ConcreteCommunicationProtocolType>::construct,
                 &TCreator<ConcreteCommunicationProtocolType>::constructInput,
                 &TCreator<ConcreteCommunicationProtocolType>::constructOutput,
                 name )
    {
    }

    static std::unique_ptr<CommunicationProtocolBase> construct( ParameterType const & paramType,
                                                                 ParameterConfigBase const & config )
    {
      return std::unique_ptr<CommunicationProtocolBase>( new ConcreteCommunicationProtocolType(paramType, config ));
    }

    static std::unique_ptr<CommunicationProtocolBase::Input> constructInput()
    {
      return std::unique_ptr<CommunicationProtocolBase::Input>( new typename ConcreteCommunicationProtocolType::InputBase() );
    }
    static std::unique_ptr<typename CommunicationProtocolBase::Output> constructOutput()
    {
      return std::unique_ptr<CommunicationProtocolBase::Output>( new typename ConcreteCommunicationProtocolType::OutputBase() );
    }
  };

  using KeyType = CommunicationProtocolType;

  using CreatorTable = std::map< KeyType, Creator >;

  static CreatorTable & creatorTable();
};

template< class ConcreteCommunicationProtocolType >
void CommunicationProtocolFactory::registerCommunicationProtocol( CommunicationProtocolType const & protocolType, char const * name )
{
  registerCommunicationProtocol( protocolType, TCreator<ConcreteCommunicationProtocolType>(name) );
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
