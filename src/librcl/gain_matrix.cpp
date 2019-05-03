/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "gain_matrix.hpp"

#include <libpml/matrix_parameter.hpp>

#include <ciso646>

namespace visr
{
namespace rcl
{

  GainMatrix::GainMatrix( SignalFlowContext const & context,
                          char const * name,
                          CompositeComponent * parent /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mInput( "in", *this )
 , mOutput( "out", *this )
{
}

void GainMatrix::setup( std::size_t numberOfInputs,
    std::size_t numberOfOutputs,
    std::size_t interpolationSteps,
    SampleType initialGain /*= static_cast<SampleType>(0.0)*/,
    bool controlInput /* = true */ )
{
  mInput.setWidth( numberOfInputs );
  mOutput.setWidth( numberOfOutputs );
  mInputChannels.resize( numberOfInputs, nullptr );
  mOutputChannels.resize( numberOfOutputs, nullptr );
  mMatrix.reset( new rbbl::GainMatrix<SampleType>( numberOfInputs, numberOfOutputs, period(), interpolationSteps, initialGain, cVectorAlignmentSamples ) );
  if( controlInput )
  {
    mGainInput.reset( new ParameterInput<pml::SharedDataProtocol, pml::MatrixParameter<SampleType> >( "gainInput", *this,
      pml::MatrixParameterConfig( numberOfOutputs, numberOfInputs ) ) );
  }
}

void GainMatrix::setup( std::size_t numberOfInputs,
                        std::size_t numberOfOutputs,
                        std::size_t interpolationSteps,
                        efl::BasicMatrix< SampleType > const & initialGains,
                        bool controlInput /* = true */)
{
  if( (initialGains.numberOfColumns() != numberOfInputs) or( initialGains.numberOfRows() != numberOfOutputs ) )
  {
    throw std::logic_error( "GainMatrix::setup: The matrix of initial gains does not match the dimensions of this object." );
  }
  mInput.setWidth( numberOfInputs );
  mOutput.setWidth( numberOfOutputs );
  mInputChannels.resize(numberOfInputs, nullptr );
  mOutputChannels.resize( numberOfOutputs, nullptr );
  mMatrix.reset( new rbbl::GainMatrix<SampleType>( numberOfInputs, numberOfOutputs, period( ),
                 interpolationSteps, initialGains, cVectorAlignmentSamples ) );
  if( controlInput )
  {
    mGainInput.reset( new ParameterInput<pml::SharedDataProtocol, pml::MatrixParameter<SampleType> >( "gainInput", *this,
                      pml::MatrixParameterConfig( numberOfOutputs, numberOfInputs ) ) );
  }
}

void GainMatrix::process()
{
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

  mMatrix->process( &mInputChannels[0], &mOutputChannels[0] );
}

} // namespace rcl
} // namespace visr
