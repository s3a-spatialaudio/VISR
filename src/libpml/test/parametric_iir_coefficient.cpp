/* Copyright Institue of Sound and Vibration Research - All rights reserved. */

#include <libpml/parametric_iir_coefficient.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <algorithm>
#include <ciso646>
#include <limits>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace pml
{
namespace test
{

BOOST_AUTO_TEST_CASE( InstantiateIirParameter )
{
  ParametricIirCoefficient<float> const p1DefaultType;
  BOOST_CHECK( p1DefaultType.type() == ParametricIirCoefficientBase::Type::allpass );
  BOOST_CHECK( p1DefaultType.gain() == 0.0f );
  
}

BOOST_AUTO_TEST_CASE( InstantiatIirParameterFromJson )
{
  ParametricIirCoefficient<float> const p1( ParametricIirCoefficient<float>::
    fromJson( std::string("{ \"type\": \"lowpass\", \"f\": 1500, \"q\": 0.707, \"gain\": 0.375 }") ));
// fromJson( std::string( "{ \"gain\": 0.375 }" ) ) );
  BOOST_CHECK( p1.type() == ParametricIirCoefficientBase::Type::lowpass );
  BOOST_CHECK_CLOSE( p1.gain(), 0.375f, std::numeric_limits<float>::epsilon() );
}


} // namespace test
} // namespace pml
} // namespace visr
