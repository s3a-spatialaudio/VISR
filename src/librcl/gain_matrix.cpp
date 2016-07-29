/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "gain_matrix.hpp"

#include <libpml/matrix_parameter.hpp>

#include <ciso646>

namespace visr
{
namespace rcl
{

  GainMatrix::GainMatrix( ril::SignalFlowContext& context,
                          char const * name,
                          ril::CompositeComponent * parent /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mInput( "in", *this )
 , mOutput( "out", *this )
{
}

void GainMatrix::setup( std::size_t numberOfInputs,
    std::size_t numberOfOutputs,
    std::size_t interpolationSteps,
    SampleType initialGain /*= static_cast<SampleType>(0.0)*/ )
{
  mNumberOfInputs = numberOfInputs;
  mNumberOfOutputs = numberOfOutputs;
  mInput.setWidth( mNumberOfInputs );
  mOutput.setWidth( mNumberOfOutputs );
  mMatrix.reset( new rbbl::GainMatrix<SampleType>( mNumberOfInputs, mNumberOfOutputs, period(), interpolationSteps, initialGain, ril::cVectorAlignmentSamples ) );
  mGainInput.reset( new ril::ParameterInputPort<pml::SharedDataProtocol, pml::MatrixParameter<SampleType> >( *this, "gainInput",
    pml::MatrixParameterConfig( mNumberOfOutputs, mNumberOfInputs ) ) );
}

void GainMatrix::setup( std::size_t numberOfInputs,
                        std::size_t numberOfOutputs,
                        std::size_t interpolationSteps,
                        efl::BasicMatrix< SampleType > const & initialGains )
{
  if( (initialGains.numberOfColumns() != numberOfInputs) or( initialGains.numberOfRows() != numberOfOutputs ) )
  {
    throw std::logic_error( "GainMatrix::setup: The matrix of initial gains does not match the dimensions of this object." );
  }
  mNumberOfInputs = numberOfInputs;
  mNumberOfOutputs = numberOfOutputs;
  mInput.setWidth( mNumberOfInputs );
  mOutput.setWidth( mNumberOfOutputs );
  mMatrix.reset( new rbbl::GainMatrix<SampleType>( mNumberOfInputs, mNumberOfOutputs, period( ),
                 interpolationSteps, initialGains, ril::cVectorAlignmentSamples ) );
  mGainInput.reset( new ril::ParameterInputPort<pml::SharedDataProtocol, pml::MatrixParameter<SampleType> >( *this, "gainInput",
                    pml::MatrixParameterConfig( mNumberOfOutputs, mNumberOfInputs ) ) );
}

void GainMatrix::process()
{
  // TODO: Adapt logic to reset the gain matrix only after it has been actually changed. 
  mMatrix->setNewGains( mGainInput->data() );

  // Allow for either zero inputs or outputs although the getVector() methods are not safe to use in this case.
  if( mInput.width() == 0 or mOutput.width() == 0 )
  {
    return;
  }
  SampleType const * const * inputVector = mInput.getVector();
  SampleType * const * outputVector = mOutput.getVector( );

  mMatrix->process( inputVector, outputVector );
}

} // namespace rcl
} // namespace visr
