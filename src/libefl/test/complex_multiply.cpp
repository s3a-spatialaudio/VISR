/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libefl/vector_functions.hpp>

#include <libefl/aligned_array.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cmath>
#include <complex>
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

  std::size_t vecSize = 16;
  efl::AlignedArray < std::complex<float> > a( vecSize, alignment );
  efl::AlignedArray < std::complex<float> > b( vecSize, alignment );

  efl::AlignedArray < std::complex<float> > c( vecSize, alignment );

  for( std::size_t runIdx( 0 ); runIdx < vecSize; ++runIdx )
  {
    a[runIdx] = std::complex<float>( static_cast<float>(runIdx), -static_cast<float>(runIdx) );
    b[runIdx] = std::complex<float>( -2.0f*static_cast<float>(runIdx), 3.0f*static_cast<float>(runIdx) );
  }

  std::fill_n( c.data(), vecSize, 0.0f );

  efl::ErrorCode const res = efl::vectorMultiply( a.data(), b.data(), c.data(), vecSize, alignment );
  BOOST_CHECK( res == efl::noError );

  efl::AlignedArray < std::complex<float> > ref( vecSize, alignment );
  std::transform( a.data( ), a.data( ) + vecSize, b.data( ), ref.data( ), [=]( std::complex<float> const & x, std::complex<float> const & y ) { return x * y; } );


}

} // namespace test
} // namespace rbbl
} // namespace visr
