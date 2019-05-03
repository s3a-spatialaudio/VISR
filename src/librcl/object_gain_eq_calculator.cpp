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
                                                CompositeComponent * parent,
                                                std::size_t numberOfObjectChannels,
                                                std::size_t numberOfBiquadSections )
  : AtomicComponent( context, name, parent )
  , mObjectInput( "objectIn", *this, pml::EmptyParameterConfig() )
  , mGainOutput( "gainOut", *this, pml::VectorParameterConfig( numberOfObjectChannels ) )
  , mEqOutput( "eqOut", *this, pml::MatrixParameterConfig( numberOfObjectChannels,
                               numberOfBiquadSections ) )
  , cNumberOfObjectChannels( numberOfObjectChannels )
  , cNumberOfBiquadSections( numberOfBiquadSections )
  , cSamplingFrequency(samplingFrequency() )
{
}

ObjectGainEqCalculator::~ObjectGainEqCalculator()
{
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
  if( objectSignalGains.size() != cNumberOfObjectChannels )
  {
    throw std::invalid_argument( "ObjectGainEqCalculator::process(): The parameter \"objectSignalGains\" must hold numberOfObjectChannels elements" );
  }
  if( (objectChannelEqs.numberOfFilters() != cNumberOfObjectChannels)
    or (objectChannelEqs.numberOfSections() != cNumberOfBiquadSections ) )
  {
    throw std::invalid_argument( "ObjectGainEqCalculator::process(): The parameter \"objectChannelEqs\" must have a matrix dimension of numberOfObjectChannels x numberOfBiquadSections" );
  }
  objectSignalGains.zeroFill();
  // TODO: Make sure that the EQs of unused channels are set to a neutral value.
  for( Object const & obj : objects )
  {
    LevelType const objLevel = obj.level();
    std::size_t const numObjChannels = obj.numberOfChannels();
    for( std::size_t chIdx(0); chIdx < numObjChannels; ++chIdx )
    {
        std::size_t const signalChannelIdx = obj.channelIndex( chIdx );
        if( signalChannelIdx >= cNumberOfObjectChannels )
        {
          status( StatusMessage::Error, "Signal index ", signalChannelIdx, " exceeds number of object signal channels (", cNumberOfObjectChannels, ")" );
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
