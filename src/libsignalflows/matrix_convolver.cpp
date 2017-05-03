/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "matrix_convolver.hpp"

#include <algorithm>
#include <vector>

namespace visr
{
namespace signalflows
{

MatrixConvolver::MatrixConvolver( SignalFlowContext const & context,
                                  char const * name,
                                  CompositeComponent * parent,
                                  std::size_t numberOfInputs,
                                  std::size_t numberOfOutputs,
                                  std::size_t filterLength,
                                  std::size_t maxFilters,
                                  std::size_t maxRoutings,
                                  efl::BasicMatrix<SampleType> const & initialFilters,
                                  pml::FilterRoutingList const & initialRoutings,
                                  char const * fftImplementation )
 : CompositeComponent( context, name, parent )
 , mAudioIn( "in", *this, numberOfInputs )
 , mAudioOut( "out", *this, numberOfOutputs )
 , mConvolver( context, "Convolver", this )
{
  mConvolver.setup( numberOfInputs, numberOfOutputs, filterLength, 
                    maxFilters, maxRoutings, initialFilters, initialRoutings, fftImplementation );
  audioConnection( mAudioIn, mConvolver.audioPort("in") );
  audioConnection( mConvolver.audioPort("out"), mAudioOut );
}

MatrixConvolver::~MatrixConvolver( )
{
}
 

} // namespace signalflows
} // namespace visr
