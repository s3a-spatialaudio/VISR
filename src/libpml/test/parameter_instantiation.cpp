/* Copyright Institue of Sound and Vibration Research - All rights reserved. */


#include <libpml/matrix_parameter.hpp>
#include <libpml/matrix_parameter_config.hpp>

#include <libril/parameter_type.hpp>
#include <libril/parameter_factory.hpp>

#include <boost/test/unit_test.hpp>

#include <ciso646>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace pml
{
namespace test
{

BOOST_AUTO_TEST_CASE( instantiateFloatMatrix )
{
  std::size_t numRowsInit = 2;
  std::size_t numColumnsInit = 3;
  MatrixParameterConfig const cfg( numRowsInit, numColumnsInit );

  std::unique_ptr<ril::ParameterBase> param = ril::ParameterFactory::create(ril::ParameterType::MatrixDouble, cfg);

  MatrixParameter<double> const & mtxParam = dynamic_cast<MatrixParameter<double> const &>(*param);

  std::size_t const numRows = mtxParam.numberOfRows();
  std::size_t const numColumns = mtxParam.numberOfColumns( );

  BOOST_CHECK( numRows == numRowsInit );
  BOOST_CHECK( numColumns == numColumnsInit );
}


} // namespace test
} // namespace pml
} // namespace visr
