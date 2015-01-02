/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_OBJECT_FACTORY_HPP_INCLUDED
#define VISR_OBJECTMODEL_OBJECT_FACTORY_HPP_INCLUDED

#include "audio_object.hpp"
#include "object_type.hpp"

#include <boost/function.hpp>

#include <map>
#include <memory>

namespace visr
{
namespace objectmodel
{

class ObjectFactory
{
public:
  static std::unique_ptr<AudioObject> create( ObjectTypeId typeId );

  template< class ObjectType >
  static void registerObjectType( ObjectTypeId typeId );

private:
  struct Creator
  {
//    typedef boost::function< AudioObject* ()> CreateFunction;
    using CreateFunction = boost::function< AudioObject* ()>;

    explicit Creator( CreateFunction fcn );

    AudioObject* create() const;
  private:
    CreateFunction mCreateFunction;
  };

  template< class ObjectType >
  class TCreator: public Creator
  {
  public:
    TCreator()
      : Creator( &TCreator<ObjectType>::construct )
    {
    }

    static AudioObject* construct()
    {
      return new ObjectType();
    }
  };

  using CreatorTable = std::map<ObjectTypeId, Creator >;

  static CreatorTable & creatorTable();
};

template< class ObjectType >
inline void ObjectFactory::registerObjectType( ObjectTypeId typeId )
{
  creatorTable().insert( std::make_pair( typeId, TCreator<ObjectType>() ) );
}

} // namespace objectmodel
} // namespace visr

#endif // #ifndef VISR_OBJECTMODEL_OBJECT_FACTORY_HPP_INCLUDED
