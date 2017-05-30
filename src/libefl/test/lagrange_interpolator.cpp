/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libefl/lagrange_interpolator.hpp>

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
namespace efl
{
namespace test
{

BOOST_AUTO_TEST_CASE( instantiateL7 )
{
  constexpr std::size_t order = 7;

  using InterType = efl::LagrangeInterpolator<float, order>;

  InterType lagInt;

  BOOST_CHECK( InterType::storageSize() == 15 );
}

} // namespace test
} // namespace rbbl
} // namespace visr
