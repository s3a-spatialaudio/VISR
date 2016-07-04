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
  MatrixParameterConfig const cfg(2, 3);

  std::unique_ptr<ril::ParameterBase> param = ril::ParameterFactory::create(ril::ParameterType::MatrixDouble, cfg);

  MatrixParameter<double> const & mtxParam = dynamic_cast<MatrixParameter<double> const &>(*param);

  std::size_t const numRows = mtxParam.numberOfRows();
  std::size_t const numColumns = mtxParam.numberOfColumns( );
}


} // namespace test
} // namespace pml
} // namespace visr
