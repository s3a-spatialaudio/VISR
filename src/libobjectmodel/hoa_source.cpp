/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "hoa_source.hpp"

namespace visr
{
namespace objectmodel
{

HoaSource::HoaSource( )
 : Object( )
 , mOrder( 0 )
{
}


HoaSource::HoaSource( ObjectId id )
 : Object( id )
 , mOrder( 0 )
{
}

/*virtual*/ HoaSource::~HoaSource()
{
}

/*virtual*/ ObjectTypeId
HoaSource::type() const
{
  return ObjectTypeId::HoaSource;
}

/*virtual*/ std::unique_ptr<Object>
HoaSource::clone() const
{
  return std::unique_ptr<Object>( new HoaSource( *this ) );
}

void HoaSource::setOrder( HoaSource::Order newOrder )
{
  mOrder = newOrder;
}

} // namespace objectmodel
} // namespace visr
