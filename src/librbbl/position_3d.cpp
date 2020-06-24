/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "position_3d.hpp"

#include "quaternion.hpp"

#include <array>
#include <algorithm>
#include <ciso646>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace visr
{
namespace rbbl
{

template< typename CoordinateType >
Position3D< CoordinateType >::Position3D() : mPos{ 0.0f, 0.0f, 0.0f }
{
}

template< typename CoordinateType >
Position3D< CoordinateType >::Position3D( CoordinateType x, CoordinateType y,
                                          CoordinateType z )
 : mPos{ x, y, z }
{
}

template< typename CoordinateType >
Position3D< CoordinateType >::Position3D( Position3D const& rhs ) = default;

template< typename CoordinateType >
Position3D< CoordinateType >::Position3D( Position3D&& rhs ) = default;

template< typename CoordinateType >
Position3D< CoordinateType >& Position3D< CoordinateType >::operator=(
    Position3D const & rhs ) = default;

template< typename CoordinateType >
Position3D< CoordinateType >& Position3D< CoordinateType >::operator=(
    Position3D&& rhs ) = default;

template< typename CoordinateType >
Position3D< CoordinateType >::~Position3D() = default;

template< typename CoordinateType >
CoordinateType Position3D< CoordinateType >::norm() const
{
  return std::sqrt( normSquare() );
}

template< typename CoordinateType >
CoordinateType Position3D< CoordinateType >::normSquare() const
{
  return mPos[ 0 ] * mPos[ 0 ] + mPos[ 1 ] * mPos[ 1 ] + mPos[ 2 ] * mPos[ 2 ];
}

template< typename CoordinateType >
void Position3D< CoordinateType >::normalise( bool silentDivideByZero )
{
  rbbl::normalise( *this, silentDivideByZero );
}

template< typename CoordinateType >
Position3D< CoordinateType > & Position3D< CoordinateType >::
operator+=( Position3D< CoordinateType > const& rhs )
{
  mPos[ 0 ] += rhs.mPos[ 0 ];
  mPos[ 1 ] += rhs.mPos[ 1 ];
  mPos[ 2 ] += rhs.mPos[ 2 ];
  return *this;
}

template< typename CoordinateType >
Position3D< CoordinateType > & Position3D< CoordinateType >::operator-=(
    Position3D< CoordinateType > const& rhs )
{
  mPos[ 0 ] -= rhs.mPos[ 0 ];
  mPos[ 1 ] -= rhs.mPos[ 1 ];
  mPos[ 2 ] -= rhs.mPos[ 2 ];
  return *this;
}

template< typename CoordinateType >
Position3D< CoordinateType > & Position3D< CoordinateType >::operator*=( CoordinateType scale )
{
  mPos[ 0 ] *= scale;
  mPos[ 1 ] *= scale;
  mPos[ 2 ] *= scale;
  return *this;
}

template< typename CoordinateType >
void Position3D< CoordinateType >::
rotate( Quaternion< CoordinateType > const& rot )
{
  Quaternion< CoordinateType > const asQuat{ 
    static_cast< CoordinateType >( 0.0 ), x(), y(), z() };
  Quaternion< CoordinateType > const resQuat = rot * asQuat * conjugate( rot );
  mPos[0] = resQuat.x();
  mPos[1] = resQuat.y();
  mPos[2] = resQuat.z();
}

template< typename CoordinateType >
void Position3D< CoordinateType >::
translate( Position3D< CoordinateType > const& shift )
{
  operator+=( shift );
}

template< typename CoordinateType >
void Position3D< CoordinateType >::transform( Quaternion< CoordinateType > const& rot,
                Position3D< CoordinateType > const& shift )
{
  rotate( rot );
  translate( shift );
}


// Implementation of free functions

template< typename CoordinateType >
CoordinateType angle( Position3D< CoordinateType > const& lhs,
                      Position3D< CoordinateType > const& rhs )
{
  try
  {
    return angleNormalised( normalise( lhs ), normalise( rhs ) );
  }
  catch( std::runtime_error const & )
  {
    throw std::runtime_error(
        "Position3D::angle() failed: One vector too close to zero." );
  }
}

template< typename CoordinateType >
CoordinateType angleNormalised( Position3D< CoordinateType > const& lhs,
                                Position3D< CoordinateType > const& rhs )
{
  CoordinateType const dotP( dot( lhs, rhs ) );
  // Protect against numerical errors causing dot products slighly outside +-1
  CoordinateType const clipped( std::max( static_cast<CoordinateType>( -1.0 ),
    std::min( static_cast< CoordinateType >( 1.0 ), dotP ) ) );
  return std::acos( clipped );
}

template< typename CoordinateType >
Position3D< CoordinateType > operator+(
    Position3D< CoordinateType > const& lhs,
    Position3D< CoordinateType > const& rhs )
{
  Position3D< CoordinateType > ret{ lhs.x() + rhs.x(), lhs.y() + rhs.z(),
                                    lhs.z() + rhs.z() };
  return ret;
}

template< typename CoordinateType >
Position3D< CoordinateType > operator-(
    Position3D< CoordinateType > const& lhs,
    Position3D< CoordinateType > const& rhs )
{
  Position3D< CoordinateType > ret{ lhs.x() - rhs.x(), lhs.y() - rhs.y(),
                                    lhs.z() - rhs.z() };
  return ret;
}

template< typename CoordinateType >
Position3D< CoordinateType > operator-(
    Position3D< CoordinateType > const& val )
{
  Position3D< CoordinateType > ret{ -val.x(), -val.y(), -val.z() };
  return ret;
}

template< typename CoordinateType >
Position3D< CoordinateType > operator*(
    CoordinateType scale, Position3D< CoordinateType > const& val )
{
  Position3D< CoordinateType > ret{ scale * val.x(), scale * val.y(),
                                    scale * val.z() };
  return ret;
}

template< typename CoordinateType >
Position3D< CoordinateType > operator/(
    Position3D< CoordinateType > const& val, CoordinateType scale )
{
  return operator*( 1.0f / scale, val );
}

template< typename CoordinateType >
Position3D< CoordinateType > normalise(
    Position3D< CoordinateType > const& val,
    bool silentDivideByZero )
{
  CoordinateType const n2{ val.norm() };
  if( (not silentDivideByZero) and 
    (n2 < std::numeric_limits< CoordinateType >::epsilon() ))
  {
    throw std::runtime_error( "Position3D::normalise(): Vector too close to zero.");
  }
  return operator*( 1.0f/n2, val );
}

template< typename CoordinateType >
CoordinateType dot( Position3D< CoordinateType > const& op1,
                    Position3D< CoordinateType > const& op2 )
{
  return op1.x() * op2.x() + op1.y() * op2.y() + op1.z() * op2.z();
}

template< typename CoordinateType >
Position3D< CoordinateType > rotate( Position3D< CoordinateType > const& pos,
                       Quaternion< CoordinateType > const& rot )
{
  Position3D< CoordinateType > ret{ pos };
  ret.rotate( rot );
  return ret;
}

template< typename CoordinateType >
Position3D< CoordinateType > transform( Position3D< CoordinateType > const& pos,
                                        Quaternion< CoordinateType > const& rot,
                                        Position3D< CoordinateType > const& shift )
{
  Position3D< CoordinateType > ret{ pos };
  ret.transform( rot, shift );
  return ret;
}

// Explicit instantiations
template class Position3D< float >;
template class Position3D< double >;


template float angle< float >( Position3D< float  > const &,
                      Position3D< float  > const & );
template double angle< double >( Position3D< double  > const &,
                      Position3D< double  > const & );

template float angleNormalised< float >( Position3D< float  > const&,
                                Position3D< float > const &);
template double angleNormalised< double >( Position3D< double  > const&,
                                Position3D< double > const &);

template Position3D< float > operator+< float >(
    Position3D< float > const&, Position3D< float > const& );
template Position3D< double > operator+< double >(
    Position3D< double > const&, Position3D< double > const& );

template Position3D< float > operator-< float >(
    Position3D< float > const&, Position3D< float > const& );
template Position3D< double > operator-< double >(
    Position3D< double > const&, Position3D< double > const& );

template Position3D< float > operator-< float >( Position3D< float > const& );
template Position3D< double > operator-< double >( Position3D< double > const& );

template Position3D< float > operator*< float >( float scale, Position3D< float > const& );
template Position3D< double > operator*< double >( double scale, Position3D< double > const& );

template Position3D< float > operator/< float >( Position3D< float > const& val, float );
template Position3D< double > operator/< double >( Position3D< double > const& val, double );

template Position3D< float > normalise< float >( Position3D< float > const&, bool );
template Position3D< double > normalise< double >( Position3D< double > const&, bool );

template float dot<float>( Position3D< float > const&, Position3D< float > const& op2 );
template double dot<double>( Position3D< double > const&, Position3D< double > const& op2 );

template Position3D< float > rotate< float >(
   Position3D< float > const &, Quaternion< float > const & );
template Position3D< double > rotate< double >(
   Position3D< double > const &, Quaternion< double > const & );

template Position3D< float > transform< float >(
  Position3D< float > const&, Quaternion< float > const&,
  Position3D< float > const& );
template Position3D< double > transform< double >(
  Position3D< double > const&, Quaternion< double > const&,
  Position3D< double > const& );

} // namespace rbbl
} // namespace visr
