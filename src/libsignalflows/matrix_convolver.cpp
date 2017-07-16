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
                                  bool controlInputs,
                                  char const * fftImplementation )
 : CompositeComponent( context, name, parent )
 , mAudioIn( "in", *this, numberOfInputs )
 , mAudioOut( "out", *this, numberOfOutputs )
 , mConvolver( context, "Convolver", this )
{
  mConvolver.setup( numberOfInputs, numberOfOutputs, filterLength, 
                    maxFilters, maxRoutings, initialFilters, initialRoutings,
                    controlInputs, fftImplementation );
  audioConnection( mAudioIn, mConvolver.audioPort("in") );
  audioConnection( mConvolver.audioPort("out"), mAudioOut );

  if( controlInputs )
  {
    mFilterInput.reset( new ParameterInput<pml::DoubleBufferingProtocol, pml::IndexedValueParameter<std::size_t, std::vector<SampleType> > >
      ( "filterInput", *this, pml::EmptyParameterConfig() ) );
    parameterConnection( *mFilterInput, mConvolver.parameterPort("filterInput") );
  }
}

MatrixConvolver::~MatrixConvolver( )
{
}
 

} // namespace signalflows
} // namespace visr
