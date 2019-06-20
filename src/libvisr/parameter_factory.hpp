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


  template< class ConcreteParameterType >
  static void unregisterParameterType(ParameterType const &  type) noexcept;

  template< class ConcreteParameterType >
  static void unregisterParameterType() noexcept;

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
  struct VISR_CORE_LIBRARY_SYMBOL Creator
  {
    using CreateFunction = std::function< ParameterBase* ( ParameterConfigBase const & config ) >;

    /**
     * Constructor, taking a creation function as parameter.
     */
    explicit Creator( CreateFunction fcn );

    /**
     * Move constructor, used to pass a Creator object into the creator table without copy construction.
     */
    Creator( Creator && rhs );

    /**
     * Deleted copy constructor to prohibit pass-by-value.
     */
    Creator( Creator const & rhs ) = delete;

    ~Creator();

    std::unique_ptr<ParameterBase> create( ParameterConfigBase const & config ) const;
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

    ~TCreator()
    {

    }

    TCreator(TCreator const & rhs) = delete;

    TCreator(TCreator && rhs) = default;

    static ParameterBase* construct( ParameterConfigBase const & config )
    {
      ParameterBase* obj = new ConcreteParameterType( config );
      return obj;
    }
  };

  struct CreatorTable : public std::map<ParameterType, Creator >
  {
    CreatorTable()
    {
    }

    ~CreatorTable()
    {
    }

  };

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
  registerParameterType<TypedParameterType>(TypedParameterType::staticType());
}

template< class ConcreteParameterType >
void ParameterFactory::unregisterParameterType(ParameterType const & type) noexcept
{
  auto & table = creatorTable();
  auto findIt = table.find( type );
  if (findIt != table.end())
  {
    table.erase( findIt );
  }
}

template< class ConcreteParameterType >
void ParameterFactory::unregisterParameterType() noexcept
{
  unregisterParameterType<ConcreteParameterType>(ConcreteParameterType::staticType());
}

// The macro does not work for multiple uses in the same .cpp file
// (multiple definitions of 'maker'), stringization of names difficult
// because of template brackets and namespace names.
// #define REGISTER_PARAMETER( type, id ) namespace { static ParameterFactory::Registrar< type > maker( id ); }

} // namespace visr

#endif // #ifndef VISR_PARAMETER_FACTORY_HPP_INCLUDED
