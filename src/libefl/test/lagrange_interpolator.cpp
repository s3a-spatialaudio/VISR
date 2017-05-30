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

  bool const reverseCoefficients = true;

  using InterType = efl::LagrangeInterpolator<float, order, reverseCoefficients>;

  InterType lagInt;

  float const mu = -0.25;

  std::array<float, order+1> res;
  lagInt.calculateCoefficients( mu, res );

  std::cout << "storage size: " << InterType::storageSize() << std::endl;
}

} // namespace test
} // namespace rbbl
} // namespace visr
