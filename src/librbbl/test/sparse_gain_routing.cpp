/* Copyright Institue of Sound and Vibration Research - All rights reserved. */

#include <librbbl/sparse_gain_routing.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <ciso646>
#include <limits>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace rbbl
{
namespace test
{

BOOST_AUTO_TEST_CASE( InstantiateSparseGainRouting )
{
  using namespace visr::rbbl;

  SparseGainRouting::IndexType const entryIdx{ 0 };
  SparseGainRouting::IndexType const inIdx{ 2 };
  SparseGainRouting::IndexType const outIdx{ 5 };
  float const gain = 0.375f;


  SparseGainRouting const routing{ entryIdx, inIdx, outIdx, gain };

  BOOST_CHECK( routing.entryIndex == entryIdx );

}

} // namespace test
} // namespace rbbl
} // namespace visr
