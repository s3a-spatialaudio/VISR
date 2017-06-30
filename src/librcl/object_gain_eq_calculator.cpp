/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "object_gain_eq_calculator.hpp"

#include <libefl/basic_vector.hpp>

#include <libobjectmodel/object_vector.hpp>

#include <librbbl/biquad_coefficient.hpp>
#include <librbbl/parametric_iir_coefficient_calculator.hpp>

#include <ciso646>

namespace visr
{
namespace rcl
{

ObjectGainEqCalculator::ObjectGainEqCalculator( SignalFlowContext const & context,
                                                char const * name,
                                                CompositeComponent * parent )
  : AtomicComponent( context, name, parent )
  , mObjectInput( "objectIn", *this, pml::EmptyParameterConfig() )
  , mGainOutput( "gainOut", *this )
  , mEqOutput( "eqOut", *this )
  , mNumberOfObjectChannels( 0 )
  , cSamplingFrequency(samplingFrequency() )
{
}

ObjectGainEqCalculator::~ObjectGainEqCalculator()
{
}

void ObjectGainEqCalculator::setup( std::size_t numberOfObjectChannels,
                                    std::size_t numberOfBiquadSections )
{
  mNumberOfObjectChannels = numberOfObjectChannels;
  mNumberOfBiquadSections = numberOfBiquadSections;
  mGainOutput.setParameterConfig( pml::VectorParameterConfig( numberOfObjectChannels ) );
  mEqOutput.setParameterConfig( pml::MatrixParameterConfig( numberOfObjectChannels, 
                                                            numberOfBiquadSections ) );
}

void ObjectGainEqCalculator::process()
{
  if( mObjectInput.changed() )
  {
    pml::ObjectVector const & ov = mObjectInput.data();
    pml::VectorParameter<CoefficientType> & gains = mGainOutput.data();
    rbbl::BiquadCoefficientMatrix<CoefficientType> & eqMatrix = mEqOutput.data();
    process( ov, gains, eqMatrix );
    mGainOutput.swapBuffers();
    mEqOutput.swapBuffers();
    mObjectInput.resetChanged();
  }
}


void ObjectGainEqCalculator::process( objectmodel::ObjectVector const & objects,
                                      efl::BasicVector<CoefficientType> & objectSignalGains,
                                      rbbl::BiquadCoefficientMatrix<CoefficientType> & objectChannelEqs )
{
  using namespace objectmodel;
  rbbl::BiquadCoefficient<SampleType> defaultEq; // Neutral EQ parameters
  if( objectSignalGains.size() != mNumberOfObjectChannels )
  {
    throw std::invalid_argument( "ObjectGainEqCalculator::process(): The parameter \"objectSignalGains\" must hold numberOfObjectChannels elements" );
  }
  if( (objectChannelEqs.numberOfFilters() != mNumberOfObjectChannels) or (objectChannelEqs.numberOfSections() != mNumberOfBiquadSections ) )
  {
    throw std::invalid_argument( "ObjectGainEqCalculator::process(): The parameter \"objectChannelEqs\" must have a matrix dimension of numberOfObjectChannels x numberOfBiquadSections" );
  }
  objectSignalGains.zeroFill();
  // TODO: Make sure that the EQs of unused channels are set to a neutral value.
  for( ObjectVector::const_iterator objIt( objects.begin()); objIt != objects.end(); ++objIt )
  {
    Object const & obj = *(objIt->second);
    LevelType const objLevel = obj.level();
    std::size_t const numObjChannels = obj.numberOfChannels();
    for( std::size_t chIdx(0); chIdx < numObjChannels; ++chIdx )
    {
        std::size_t const signalChannelIdx = obj.channelIndex( chIdx );
        if( signalChannelIdx >= mNumberOfObjectChannels )
        {
          status( StatusMessage::Error, "Signal index ", signalChannelIdx, " exceeds number of object signal channels (", mNumberOfObjectChannels, ")" );
          continue;
        }
        objectSignalGains[ signalChannelIdx ] = objLevel;
        // Potential minor performance improvement possible: For multichannel objects, compute the coefficients only for the first channel,
        // and copy it to the remaining channels.
        rbbl::ParametricIirCoefficientCalculator::calculateIirCoefficients<SampleType>( obj.eqCoefficients(),
                                                                                             objectChannelEqs[signalChannelIdx],
                                                                                             static_cast<SampleType>(cSamplingFrequency) );
    }
  }
}


} // namespace rcl
} // namespace visr
