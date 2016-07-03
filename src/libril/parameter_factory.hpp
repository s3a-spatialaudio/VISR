/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_PARAMETER_FACTORY_HPP_INCLUDED
#define VISR_LIBRIL_PARAMETER_FACTORY_HPP_INCLUDED

#include "parameter_type.hpp"

#include <map>
#include <memory>
#include <string>

#include <boost/function.hpp>

namespace visr
{
namespace ril
{

// Forward declarations
class ParameterBase;
// class ParameterType;
class ParameterConfigBase;

class ParameterFactory
{
public:
  static std::unique_ptr<ParameterBase> create(ParameterType const & type, ParameterConfigBase const & config);
  // static std::unique_ptr<ParameterBase> create( ParameterType const & type, ParameterConfigBase const & config );

  template< class ConcreteParameterType >
  static void registerParameterType( ParameterType const &  type );

private:
  struct Creator
  {
    using CreateFunction = boost::function< ParameterBase* ( ParameterConfigBase const & config ) >;

    explicit Creator( CreateFunction fcn );

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

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_PARAMETER_FACTORY_HPP_INCLUDED
