/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "sparse_gain_matrix.hpp"

#include <libvisr/signal_flow_context.hpp>

#include <librbbl/sparse_gain_routing.hpp>

#include <ciso646>

namespace visr
{
namespace rcl
{

SparseGainMatrix::SparseGainMatrix( SignalFlowContext const & context,
                                    char const * name,
                                    CompositeComponent * parent,
                                    std::size_t numberOfInputs,
                                    std::size_t numberOfOutputs,
                                    std::size_t interpolationSteps,
                                    std::size_t maxRoutingPoints,
                                    rbbl::SparseGainRoutingList const & initialRoutings /* = rbbl::SparseGainRoutingList() */,
                                    ControlInputs controlInputs /*= ControlInputs::No*/ )
 : AtomicComponent( context, name, parent )
 , mGainRamp( context.period(), interpolationSteps, cVectorAlignmentSamples )
 , mInput( "in", *this, numberOfInputs )
 , mOutput( "out", *this, numberOfOutputs )
{
  //mInputChannels.resize( numberOfInputs, nullptr );
  //mOutputChannels.resize( numberOfOutputs, nullptr );
  //mMatrix.reset( new rbbl::SparseGainMatrix<SampleType>( numberOfInputs, numberOfOutputs, period(), interpolationSteps, initialGain, cVectorAlignmentSamples ) );
  if( controlInputs != ControlInputs::No )
  {
#if 0
    mGainInput.reset( new ParameterInput<pml::SharedDataProtocol, pml::MatrixParameter<SampleType> >( "gainInput", *this,
      pml::EmptyParameterConfig()) );
#endif
  }
}

void SparseGainMatrix::process()
{
#if 0
  if( mGainInput )
  {
    // TODO: Adapt logic to reset the gain matrix only after it has been actually changed. 
    // Thus would be a match for the DoubleBufferingProtocol.
    mMatrix->setNewGains( mGainInput->data() );
  }

  // Allow for either zero inputs or outputs although the getVector() methods are not safe to use in this case.
  if( mInput.width() == 0 or mOutput.width() == 0 )
  {
    return;
  }
  mInput.getChannelPointers( &mInputChannels[0] );
  mOutput.getChannelPointers( &mOutputChannels[0] );

  // mMatrix->process( &mInputChannels[0], &mOutputChannels[0] );
#endif
}

} // namespace rcl
} // namespace visr
