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
  BOOST_CHECK( p1.type() == ParametricIirCoefficientBase::Type::lowpass );
  BOOST_CHECK_CLOSE( p1.gain(), 0.375f, std::numeric_limits<float>::epsilon() );
}

BOOST_AUTO_TEST_CASE( InstantiatIirParameterFromInitializer )
{
  ParametricIirCoefficient<float> const p{ ParametricIirCoefficient<float>::Type::allpass, 1.0f, 1.0f };
  BOOST_CHECK( p.type() == ParametricIirCoefficientBase::Type::allpass );
  BOOST_CHECK_CLOSE( p.gain(), 0.0f, std::numeric_limits<float>::epsilon() );
}

BOOST_AUTO_TEST_CASE( InstantiatIirParameterListDefault )
{
  ParametricIirCoefficientList<float> const pl{};
  BOOST_CHECK( pl.empty() );
}

BOOST_AUTO_TEST_CASE( InstantiatIirParameterListVectorInit )
{
  ParametricIirCoefficientList<float> const pl( 5, ParametricIirCoefficient<float>{ ParametricIirCoefficient<float>::Type::highpass, 1.0f, 1.0f, 0.45f } );
  BOOST_CHECK( pl.size() == 5);
  for( auto c : pl )
  {
    BOOST_CHECK( c.type() == ParametricIirCoefficientBase::Type::highpass );
  }
}

BOOST_AUTO_TEST_CASE( InstantiatIirParameterListFromJson )
{
  std::string const jsonMsg = "[ { \"type\": \"lowpass\", \"f\": 1500, \"q\": 0.707, \"gain\": 0.375 },"
  "{ \"type\": \"highpass\", \"f\": 500, \"q\": 2.0, \"gain\": 0.75 }, "
    "{ \"type\": \"peak\", \"f\": 2500, \"q\": 0.5, \"gain\": 0.25 }]";

  ParametricIirCoefficientList<double> pl;
  pl.loadJson( jsonMsg );
  BOOST_CHECK( pl.size() == 3 );
}

BOOST_AUTO_TEST_CASE( InstantiatIirParameterListFromXml )
{
  std::string const xmlMsg =
  "<eqs>"
    "<eq type=\"lowpass\" f=\"1500\" q=\"0.707\" gain=\"0.375\" />"
    "<eq type=\"highpass\" f=\"500\" q=\"2.0\" gain=\"0.75\" />"
    "<eq type=\"peak\" f=\"2500\" q=\"0.5\" gain=\"0.25\" />"
  "</eqs>";

  ParametricIirCoefficientList<double> pl;
  pl.loadXml( xmlMsg );
  BOOST_CHECK( pl.size() == 3 );
}


} // namespace test
} // namespace pml
} // namespace visr
