/* Copyright Institue of Sound and Vibration Research - All rights reserved. */

#include <librbbl/kiss_fft_wrapper.hpp>

#include <libefl/basic_vector.hpp>
#include <libefl/vector_functions.hpp>
#include <libvisr/constants.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <complex>
#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

namespace visr
{
namespace rbbl
{
namespace test
{

BOOST_AUTO_TEST_CASE( KissFftWrapperSingle )
{
  std::size_t dftSize = 8;
  std::size_t outputSize = 2*(dftSize / 2 + dftSize%2 + 1);

  KissFftWrapper<float> fft( dftSize, cVectorAlignmentSamples );

  efl::BasicVector<float> input( dftSize, cVectorAlignmentSamples );
  input[1] = 1.0f;

  efl::BasicVector<std::complex<float> > output( outputSize, cVectorAlignmentSamples );

  fft.forwardTransform( input.data(), output.data() );

  efl::BasicVector<float> resultAfterInverse( dftSize, cVectorAlignmentSamples );

  fft.inverseTransform( output.data(), resultAfterInverse.data() );
}

} // namespace test
} // namespace rbbl
} // namespace visr
