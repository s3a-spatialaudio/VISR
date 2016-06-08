/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libefl/vector_functions.hpp>
#include <libefl/vector_functions_reference.hpp>

#include <libefl/aligned_array.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cmath>
#include <complex>
#include <iostream>
#include <valarray>

namespace visr
{
namespace rbbl
{
namespace test
{
            
BOOST_AUTO_TEST_CASE( complexMultiply1 )
{
  const std::size_t alignment = 0;

  efl::getCpuId();

  std::size_t vecSize = 27;
  efl::AlignedArray < std::complex<float> > a( vecSize, alignment );
  efl::AlignedArray < std::complex<float> > b( vecSize, alignment );

  efl::AlignedArray < std::complex<float> > c( vecSize, alignment );
  efl::AlignedArray < std::complex<float> > reference( vecSize, alignment );


  for( std::size_t runIdx( 0 ); runIdx < vecSize; ++runIdx )
  {
    a[runIdx] = std::complex<float>( static_cast<float>(runIdx), -static_cast<float>(runIdx) );
    b[runIdx] = std::complex<float>( -2.0f*static_cast<float>(runIdx), 3.0f*static_cast<float>(runIdx) );
  }

  std::fill_n( c.data(), vecSize, 0.0f );

  efl::ErrorCode const resRef = efl::reference::vectorMultiply( a.data( ), b.data( ), reference.data( ), vecSize, alignment );
  BOOST_CHECK( resRef == efl::noError );

  efl::ErrorCode const res = efl::vectorMultiply( a.data(), b.data(), c.data(), vecSize, alignment );
  BOOST_CHECK( res == efl::noError );

  efl::AlignedArray < std::complex<float> > ref( vecSize, alignment );
  std::transform( a.data( ), a.data( ) + vecSize, b.data( ), ref.data( ), [=]( std::complex<float> const & x, std::complex<float> const & y ) { return x * y; } );

  for( std::size_t vecIdx( 0 ); vecIdx < vecSize; ++vecIdx )
  {
    std::cout << "c[" << vecIdx << "]: " << ref[vecIdx] << " : " << reference[vecIdx] << " : " << c[vecIdx] << std::endl;
  }
}

BOOST_AUTO_TEST_CASE( complexMultiplyConstant )
{
  const std::size_t alignment = 2;

  std::size_t vecSize = 23;
  efl::AlignedArray < std::complex<float> > a( vecSize, alignment );
  std::complex<float> const c = { 0.385f, -0.75f};

  efl::AlignedArray < std::complex<float> > result( vecSize, alignment );
  efl::AlignedArray < std::complex<float> > reference( vecSize, alignment );

  for( std::size_t runIdx( 0 ); runIdx < vecSize; ++runIdx )
  {
    a[runIdx] = std::complex<float>( static_cast<float>(runIdx), -static_cast<float>(runIdx) );
  }

  efl::ErrorCode const resRef = efl::reference::vectorMultiplyConstantAddInplace( c, a.data( ), reference.data( ), vecSize, alignment );
  BOOST_CHECK( resRef == efl::noError );

  efl::ErrorCode const res = efl::vectorMultiplyConstantAddInplace( c, a.data(), result.data(), vecSize, alignment );
  BOOST_CHECK( res == efl::noError );

  for( std::size_t vecIdx( 0 ); vecIdx < vecSize; ++vecIdx )
  {
    std::cout << vecIdx << ": " << reference[vecIdx] << " : " << result[vecIdx] << std::endl;
  }

  for( std::size_t vecIdx( 0 ); vecIdx < vecSize; ++vecIdx )
  {
    BOOST_CHECK_CLOSE( std::abs(result[vecIdx] - reference[vecIdx]), 0.0f, 1.0e-6 );
  }
}

} // namespace test
} // namespace rbbl
} // namespace visr
