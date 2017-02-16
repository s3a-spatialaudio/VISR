/* Copyright Institue of Sound and Vibration Research - All rights reserved. */


#include <libril/parameter_factory.hpp>
#include <libril/parameter_type.hpp>

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

BOOST_AUTO_TEST_CASE( CheckTypePresent )
{
  std::size_t numRows = 3;
  std::size_t numCols = 7;
  
  pml::MatrixParameterConfig const mtxCfg{ numRows, numCols };

  auto const paramPtr = ParameterFactory::create( ParameterType::MatrixFloat, mtxCfg );
  BOOST_CHECK( paramPtr );
  
  pml::MatrixParameter<float> const * mtxParamPtr; // { nullptr };
  BOOST_CHECK_NO_THROW( mtxParamPtr = dynamic_cast<pml::MatrixParameter<float> const *>( paramPtr.get() ) );
  BOOST_CHECK( mtxParamPtr->numberOfRows() == numRows );

}

} // namespace test
} // namespace ril
} // namespace visr
