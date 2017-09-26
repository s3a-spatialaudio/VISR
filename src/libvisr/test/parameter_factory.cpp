/* Copyright Institue of Sound and Vibration Research - All rights reserved. */


#include <libvisr/parameter_factory.hpp>
#include <libvisr/parameter_type.hpp>

#include <libpml/matrix_parameter.hpp>
#include <libpml/matrix_parameter_config.hpp>

#include <boost/test/unit_test.hpp>

#include <iostream>

namespace visr
{
namespace ril
{
namespace test
{

// That needs to go to pml
#if 0
BOOST_AUTO_TEST_CASE( CheckTypePresent )
{
  std::size_t numRows = 3;
  std::size_t numCols = 7;
  
  pml::MatrixParameterConfig const mtxCfg{ numRows, numCols };

  auto const paramPtr = ParameterFactory::create( pml::MatrixParameter<float>::staticType(), mtxCfg );
  BOOST_CHECK( paramPtr );
  
  pml::MatrixParameter<float> const * mtxParamPtr; // { nullptr };
  BOOST_CHECK_NO_THROW( mtxParamPtr = dynamic_cast<pml::MatrixParameter<float> const *>( paramPtr.get() ) );
  BOOST_CHECK( mtxParamPtr->numberOfRows() == numRows );
}
#endif

} // namespace test
} // namespace ril
} // namespace visr
