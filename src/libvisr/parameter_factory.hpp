/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_FACTORY_HPP_INCLUDED
#define VISR_PARAMETER_FACTORY_HPP_INCLUDED

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
template<class ConcreteType, class ParameterConfig, ParameterType> class TypedParameterBase;

class VISR_CORE_LIBRARY_SYMBOL ParameterFactory
{
public:
  static std::unique_ptr<ParameterBase> create(ParameterType const & type, ParameterConfigBase const & config);

  /**
   * Return the number of registered parameter types.
   */
  static std::size_t numberOfParameterTypes() noexcept;

  /**
   * Query if a parameter type with the given id is registered in the factory
   * @return True if the parameter type is registered, false otherwise.
   */
  static bool typeExists( ParameterType type ) noexcept;

  template< class ConcreteParameterType >
  static void registerParameterType( ParameterType const &  type );

  /**
   * Special registration function for ParameterType subtypes that inherit from 
   * TypedParameterBase.
   */
  template< class TypedParameterType >
  static void registerParameterType();

  /**
   * Template class to register parameter types.
   * Creating a static object invokes the class registration function.
   */
  template< class ConcreteParameterType >
  class Registrar
  {
  public:
    explicit Registrar( ParameterType type )
    {
      creatorTable().insert( std::make_pair( type, TCreator<ConcreteParameterType>() ) );
    }
  };

private:
  struct Creator
  {
    using CreateFunction = std::function< ParameterBase* ( ParameterConfigBase const & config ) >;

    VISR_CORE_LIBRARY_SYMBOL explicit Creator( CreateFunction fcn );

    VISR_CORE_LIBRARY_SYMBOL std::unique_ptr<ParameterBase> create( ParameterConfigBase const & config ) const;
 private:
    CreateFunction mCreateFunction;
  };

  template< class ConcreteParameterType >
  class TCreator: public Creator
  {
  public:
    TCreator( )
      : Creator( &TCreator<ConcreteParameterType>::construct )
    {
    }

    static ParameterBase* construct( ParameterConfigBase const & config )
    {
      ParameterBase* obj = new ConcreteParameterType( config );
      return obj;
    }
  };

  using CreatorTable = std::map<ParameterType, Creator >;

  static CreatorTable & creatorTable();
};

template< class ConcreteParameterType >
void ParameterFactory::registerParameterType( ParameterType const & type )
{
  creatorTable().insert( std::make_pair( type, TCreator<ConcreteParameterType>() ) );
}

template< class TypedParameterType >
void ParameterFactory::registerParameterType()
{
  registerParameterType<TypedParameterType>( TypedParameterType::staticType() );
}


// The macro does not work for multiple uses in the same .cpp file
// (multiple definitions of 'maker'), stringization of names difficult
// because of template brackets and namespace names.
// #define REGISTER_PARAMETER( type, id ) namespace { static ParameterFactory::Registrar< type > maker( id ); }

} // namespace visr

#endif // #ifndef VISR_PARAMETER_FACTORY_HPP_INCLUDED
