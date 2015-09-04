/* Copyright Institue of Sound and Vibration Research - All rights reserved. */

#include <librbbl/ipp_fft_wrapper.hpp>

#include <libefl/basic_vector.hpp>
#include <libefl/vector_functions.hpp>
#include <libril/constants.hpp>

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

BOOST_AUTO_TEST_CASE( IppFftWrapperSingle )
{
  std::size_t dftSize = 8;
  std::size_t outputSize = dftSize / 2 + 1;

  IppFftWrapper<float> fft( dftSize, ril::cVectorAlignmentSamples );

  efl::BasicVector<float> input( dftSize, ril::cVectorAlignmentSamples );
  input[1] = 1.0f;

  efl::BasicVector<std::complex<float> > output( outputSize, ril::cVectorAlignmentSamples );

  fft.forwardTransform( input.data(), output.data() );

  efl::BasicVector<float> resultAfterInverse( dftSize, ril::cVectorAlignmentSamples );

  fft.inverseTransform( output.data(), resultAfterInverse.data() );
}

} // namespace test
} // namespace rbbl
} // namespace visr
