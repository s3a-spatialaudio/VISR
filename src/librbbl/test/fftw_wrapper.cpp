/* Copyright Institue of Sound and Vibration Research - All rights reserved. */

#include <librbbl/fftw_wrapper.hpp>

#include <libefl/basic_vector.hpp>
#include <libefl/vector_functions.hpp>
#include <libril/constants.hpp>


#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <algorithm>
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

BOOST_AUTO_TEST_CASE( FftwWrapperSingle )
{
  std::size_t dftSize = 8;
  std::size_t outputSize = 2*(dftSize / 2 + dftSize%2 + 1);

  FftwWrapper<float> fft;
  fft.init( dftSize, ril::cVectorAlignmentSamples );

  efl::BasicVector<float> input( dftSize, ril::cVectorAlignmentSamples );
  input[1] = 1.0f;

  efl::BasicVector<float> output( outputSize, ril::cVectorAlignmentSamples );

  fft.forwardFft( input.data(), output.data() );

  efl::BasicVector<float> resultAfterInverse( dftSize, ril::cVectorAlignmentSamples );

  fft.inverseFft( output.data(), resultAfterInverse.data() );
}


} // namespace test
} // namespace rbbl
} // namespace visr
