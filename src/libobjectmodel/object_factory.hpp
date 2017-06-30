/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_OBJECT_FACTORY_HPP_INCLUDED
#define VISR_OBJECTMODEL_OBJECT_FACTORY_HPP_INCLUDED

#include "object.hpp"
#include "object_type.hpp"
#include "object_parser.hpp" // experimental support for dispatching to the correct parser.

#include "export_symbols.hpp"

#include <boost/function.hpp>

#include <map>
#include <memory>

namespace visr
{
namespace objectmodel
{

class VISR_OBJECTMODEL_LIBRARY_SYMBOL ObjectFactory
{
public:
  static std::unique_ptr<Object> create( ObjectTypeId typeId );

  static const ObjectParser & parser( ObjectTypeId typeId );

  template< class ObjectType, class ParserType >
  static void registerObjectType( ObjectTypeId typeId );

private:
  struct Creator
  {
    using CreateFunction = boost::function< Object* ()>;

    explicit Creator( CreateFunction fcn, ObjectParser * parser );

    Object* create() const;

    ObjectParser const & parser() const;
  private:
    CreateFunction mCreateFunction;

    std::shared_ptr<ObjectParser> mParser;
  };

  template< class ObjectType, class ParserType >
  class TCreator: public Creator
  {
  public:
    TCreator( )
      : Creator( &TCreator<ObjectType,ParserType>::construct, new ParserType() )
    {
    }

    static Object* construct()
    {
      return new ObjectType();
    }
  };

  using CreatorTable = std::map<ObjectTypeId, Creator >;

  static CreatorTable & creatorTable();
};

template< class ObjectType, class ParserType >
void ObjectFactory::registerObjectType( ObjectTypeId typeId )
{
  creatorTable().insert( std::make_pair( typeId, TCreator<ObjectType, ParserType>() ) );
}

} // namespace objectmodel
} // namespace visr

#endif // #ifndef VISR_OBJECTMODEL_OBJECT_FACTORY_HPP_INCLUDED
