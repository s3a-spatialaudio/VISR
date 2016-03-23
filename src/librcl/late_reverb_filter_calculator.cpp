/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "late_reverb_filter_calculator.hpp"

#include <libefl/basic_vector.hpp>

#include <libpml/message_queue.hpp>

#include <array>

#include <ciso646>
#include <random>

namespace visr
{
namespace rcl
{

namespace
{

template<typename SampleType>
efl::ErrorCode filterBiquad( SampleType const * const input, SampleType * const output, std::size_t numSamples,
                             pml::BiquadParameter<SampleType> const & iir, std::array<SampleType, 2> const & pastInputs = { 0.0f, 0.0f }, std::array<SampleType, 2> const & initialState = { 0.0f, 0.0f } )
{
  std::array<SampleType, 2> state( initialState);
  std::array<SampleType, 3> inputBuffer = {0.0f, pastInputs[0], pastInputs[1]};
  // TODO: your code here.
  for( std::size_t sampleIdx( 0 ); sampleIdx < numSamples; ++sampleIdx )
  {
    inputBuffer[0] = inputBuffer[1];
    inputBuffer[1] = inputBuffer[0];
    inputBuffer[2] = input[sampleIdx];
    ril::SampleType const y = iir.b0()*inputBuffer[2] + iir.b1()*inputBuffer[1] + iir.b2 ()*inputBuffer[0] - iir.a1()*state[1] - iir.a2 ()*state[0];
    output[sampleIdx]=y;

    state[0] = state[1];
    state[1] = y;
  }

  return efl::noError;
}

// Explicit instantiations
template efl::ErrorCode filterBiquad( float const * const input, float * const output, std::size_t numSamples,
  pml::BiquadParameter<float> const & iir, std::array<float, 2> const &, std::array<float, 2> const & );
template efl::ErrorCode filterBiquad( double const * const input, double * const output, std::size_t numSamples,
  pml::BiquadParameter<double> const & iir, std::array<double, 2> const &, std::array<double, 2> const &);


} // unnamed namespace

LateReverbFilterCalculator::LateReverbFilterCalculator( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
 , mAlignment( ril::cVectorAlignmentSamples )
 , mSubBandNoiseSequences( mAlignment )
{
}

LateReverbFilterCalculator::~LateReverbFilterCalculator()
{
}

void LateReverbFilterCalculator::setup( std::size_t numberOfObjects,
                                        ril::SampleType lateReflectionLengthSeconds,
                                        std::size_t numLateReflectionSubBandLevels )
{
  mNumberOfObjects = numberOfObjects;
  mNumberOfSubBands = numLateReflectionSubBandLevels;
  mFilterLength = static_cast<std::size_t>( std::ceil( lateReflectionLengthSeconds * flow().samplingFrequency() ) );

  mSubBandNoiseSequences.resize( numberOfObjects * mNumberOfSubBands, mFilterLength );
}

void LateReverbFilterCalculator::process( SubBandMessageQueue & subBandLevels,
  LateFilterMessageQueue & lateFilters )
{
  while( not subBandLevels.empty() )
  {
    SubBandMessageQueue::MessageType const & val = subBandLevels.nextElement();
    if( val.first >= mNumberOfObjects )
    {
      throw std::out_of_range( "LateReverbFilterCalculator: Object index out of range." );
    }
    std::vector<ril::SampleType> newFilter( mFilterLength );

    // Check whether the values have changed.

//    calculateFIR( val.first, val.second, newFilter );
//    lateFilters.enqueue( std::make_pair( val.first, newFilter ) );
    subBandLevels.popNextElement();
  }
}

void LateReverbFilterCalculator::
calculateImpulseResponse( std::size_t objectIdx,
                          objectmodel::PointSourceWithReverb::LateReverb const & lateParams,
                          ril::SampleType * ir,
                          std::size_t irLength, std::size_t alignment /*= 0*/ )
{
  // Do whatever needed to calculate the reverb filter

  // for each subband...
  // get a slightly too long noise sequence
  //createWhiteNoiseSequence...

  // filter the sequence, take the middle samples, and store in the large noise matrix
  //filterSequence()

  // multiply the sequence with the envelope
}

/**
* Create a uniform white noise sequence with range [-1,1].
* @param numSamples Length of the noise sequence.
* @param [out] data Buffer to store the result.
* @param alignment Alignment of the \p data buffer (in number of elements)
*/
static void createWhiteNoiseSequence( std::size_t numSamples, ril::SampleType* data, std::size_t /*alignment = 0*/ )
{

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<ril::SampleType> dis(-1.0f, 1.0f);

    
  for (int n = 0; n < numSamples; ++n) {
    data[n]=dis(gen);
  }
}

/*static*/ void LateReverbFilterCalculator::filterSequence( std::size_t numSamples, ril::SampleType const * const input, ril::SampleType * output,
  pml::BiquadParameter<ril::SampleType> const & filter )
{

    

}


/*static*/ void LateReverbFilterCalculator::createEnvelope( std::size_t numSamples, ril::SampleType* data,
                                                            ril::SampleType initialDelay, ril::SampleType gain,
                                                            ril::SampleType attackCoeff, ril::SampleType decayCoeff,
                                                            ril::SampleType samplingFrequency )
{
  // initialDelay in seconds sets leading zeros
  // attackCoeff in seconds is the length of the onset ramp
  // decayCoeff is the decay constant for the exponential (late) decay

  std::size_t const initialDelaySamples = static_cast<std::size_t>(std::round( initialDelay*samplingFrequency ));
  std::size_t attackCoeffSamples = static_cast<std::size_t>(std::round( attackCoeff*samplingFrequency ));
    

  for( std::size_t n = 0; n < initialDelaySamples; ++n )
  { // leading zeros for delay
        data[n]=0.0f;
  }
  for( std::size_t n = initialDelaySamples; n < attackCoeffSamples; ++n )
  { // linear increase up to max
    data[n] = gain * ((n - initialDelaySamples) / (attackCoeffSamples - initialDelaySamples));
  }
  for( std::size_t n = attackCoeffSamples; n < numSamples; ++n )
  { // exponential decay to end of envelope
    ril::SampleType const decay = gain * std::exp( decayCoeff*(n - attackCoeffSamples) );
    data[n] = decay;

  }
}

} // namespace rcl
} // namespace visr
