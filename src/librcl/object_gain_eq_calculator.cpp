/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "object_gain_eq_calculator.hpp"

#include <libefl/basic_vector.hpp>

#include <libobjectmodel/object_vector.hpp>

#include <libpml/biquad_parameter.hpp>

#include <ciso646>

namespace visr
{
namespace rcl
{

ObjectGainEqCalculator::ObjectGainEqCalculator( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
 , mNumberOfObjectChannels( 0 )
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
  mInternalBiquadVector.reset( new pml::BiquadParameterList<ril::SampleType >( numberOfBiquadSections ));
}

void ObjectGainEqCalculator::process( objectmodel::ObjectVector const & objects,
					     efl::BasicVector<CoefficientType> & objectSignalGains,
					     pml::BiquadParameterMatrix<CoefficientType> & objectChannelEqs )
{
  using namespace objectmodel;
  pml::BiquadParameter<ril::SampleType> defaultEq; // Neutral EQ parameters
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
    // TODO: Retrieve EQ setting from object and store calculate eq section in mInternalBiquadVector
    std::size_t const objEqSections = 0; // Interim setting until EQ calculation is in place.
      
    std::size_t const numObjChannels = obj.numberOfChannels();
    for( std::size_t chIdx(0); chIdx < numObjChannels; ++chIdx )
    {
        std::size_t const signalChannelIdx = obj.channelIndex( chIdx );
        objectSignalGains[ signalChannelIdx ] = objLevel;
        for( std::size_t eqIdx(0); eqIdx < objEqSections; ++eqIdx ) // Compiler warning 'always false' is harmless at the moment.
        {
            objectChannelEqs( signalChannelIdx, eqIdx ) = mInternalBiquadVector->at(eqIdx);
        }
        for( std::size_t eqIdx(objEqSections); eqIdx < mNumberOfBiquadSections; ++eqIdx ) // Fill remaining sections with neutral parameters.
        {
            objectChannelEqs( signalChannelIdx, eqIdx ) = defaultEq;
        }
        
    }
  }
}


} // namespace rcl
} // namespace visr
