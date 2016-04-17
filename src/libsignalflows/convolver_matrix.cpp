/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "convolver_matrix.hpp"

#include <algorithm>
#include <vector>

namespace visr
{
namespace signalflows
{

namespace
{
  // create a helper function in an unnamed namespace

  /**
  * Create a vector of unsigned integers ranging from \p start to \p end - 1.
  * @param startIdx the start index of the sequence.
  * @param endIdx The index value one past the end
  * @note Compared to other versions of this function, \p endIdx is the 'past the end' value here, as common in C++ STL conventions.
  * that is indexRange( n, n ) returns an empty vector.
  */
  std::vector<std::size_t> indexRange( std::size_t startIdx, std::size_t endIdx )
  {
    if( endIdx <= startIdx )
    {
      return std::vector<std::size_t>( );
    }
    std::size_t const vecLength( endIdx - startIdx );
    std::vector < std::size_t> ret( vecLength );
    std::generate( ret.begin( ), ret.end( ), [&] { return startIdx++; } );
    return ret;
  }
}

ConvolverMatrix::ConvolverMatrix( std::size_t numberOfInputs,
                                  std::size_t numberOfOutputs,
                                  std::size_t filterLength,
                                  std::size_t maxFilters,
                                  std::size_t maxRoutings,
                                  efl::BasicMatrix<ril::SampleType> const & initialFilters,
                                  pml::FilterRoutingList const & initialRoutings,
                                  char const * fftImplementation,
                                  std::size_t period,
                                  ril::SamplingFrequencyType samplingFrequency )
 : AudioSignalFlow( period, samplingFrequency )
 , mConvolver( *this, "Convolver" )
{
  mConvolver.setup( numberOfInputs, numberOfOutputs, filterLength, 
                    maxFilters, maxRoutings, initialFilters, initialRoutings, fftImplementation );

  initCommArea( numberOfInputs + numberOfOutputs, period, ril::cVectorAlignmentSamples );

  // connect the ports
  assignCommunicationIndices( "Convolver", "in", indexRange( 0, numberOfInputs ) );
  assignCommunicationIndices( "Convolver", "out", indexRange( numberOfInputs, numberOfInputs + numberOfOutputs ) );

  // Set the indices for communicating the signals from and to the outside world.
  assignCaptureIndices( indexRange( 0, numberOfInputs ) );
  assignPlaybackIndices( indexRange( numberOfInputs, numberOfInputs + numberOfOutputs ) );

  // should not be done here, but in AudioSignalFlow where this method is called.
  setInitialised( true );
}

ConvolverMatrix::~ConvolverMatrix( )
{
}
 
/*virtual*/ void 
ConvolverMatrix::process()
{
  // At the moment, there are no runtime parameter changes.
  mConvolver.process();
}

} // namespace signalflows
} // namespace visr
