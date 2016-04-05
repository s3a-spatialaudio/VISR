/* Copyright Institute of Sound and Vibration Research - All rights reserved */

// avoid annoying warning about unsafe STL functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

#include "reverb_parameter_calculator.hpp"

#include <libefl/basic_matrix.hpp>
#include <libefl/vector_functions.hpp>

#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/point_source_with_reverb.hpp>

#include <libpml/signal_routing_parameter.hpp>

#include <librbbl/object_channel_allocator.hpp>

#include <algorithm>
#include <cassert>
#include <ciso646>
#include <limits>
#include <vector>

namespace visr
{
namespace rcl
{

ReverbParameterCalculator::ReverbParameterCalculator( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
 , mMaxNumberOfObjects( 0 )
{
}

ReverbParameterCalculator::~ReverbParameterCalculator()
{
}

void ReverbParameterCalculator::setup( panning::LoudspeakerArray const & arrayConfig,
                                       std::size_t numberOfObjects,
                                       std::size_t numberOfDiscreteReflectionsPerSource,
                                       std::size_t numBiquadSectionsReflectionFilters,
                                       ril::SampleType lateReflectionLengthSeconds,
                                       std::size_t numLateReflectionSubBandFilters )
{
    mMaxNumberOfObjects=numberOfObjects;
    mNumberOfDiscreteReflectionsPerSource = numberOfDiscreteReflectionsPerSource;
    mNumberOfBiquadSectionsReflectionFilters = numBiquadSectionsReflectionFilters;
    mLateReflectionLengthSeconds = lateReflectionLengthSeconds;
    mNumberOfLateReflectionSubBandFilters = numLateReflectionSubBandFilters;

    //if( mNumberOfDiscreteReflectionsPerSource != objectmodel::PointSourceWithReverb::cNumDiscreteReflectionBiquads )
    if( mNumberOfBiquadSectionsReflectionFilters != objectmodel::PointSourceWithReverb::cNumDiscreteReflectionBiquads )
    {
      throw std::invalid_argument( "The number of biquad sections for the discrete reflections differs from the constant used in the object definition." );
    }

    mChannelAllocator.reset( new rbbl::ObjectChannelAllocator( mMaxNumberOfObjects ) );

    // Configure the VBAP calculator
    mSourcePositions.resize( mNumberOfDiscreteReflectionsPerSource ); // Process one reverb object at a time.
    mVbapCalculator.setNumSources( mNumberOfDiscreteReflectionsPerSource /* * mMaxNumberOfObjects */ );
    mVbapCalculator.setLoudspeakerArray( &arrayConfig );
    mVbapCalculator.setListenerPosition( 0.0f, 0.0f, 0.0f ); // Use a default listener position
    if( mVbapCalculator.calcInvMatrices( ) != 0 )
    {
      throw std::invalid_argument( "ReverbParameterCalculator::setup(): Calculation of inverse matrices for VBAP calculatorfailed." );
    }
    mNumberOfPanningLoudspeakers = mVbapCalculator.getNumSpeakers();
}

/**
* The process function.
* It takes a vector of objects as input and calculates a vector of output gains.
*/
void ReverbParameterCalculator::process( objectmodel::ObjectVector const & objects,
                                         pml::SignalRoutingParameter & signalRouting,
                                         efl::BasicVector<ril::SampleType> & discreteReflGains,
                                         efl::BasicVector<ril::SampleType> & discreteReflDelays,
                                         pml::BiquadParameterMatrix<ril::SampleType> & biquadCoeffs,
                                         efl::BasicMatrix<ril::SampleType> & discretePanningMatrix,
                                         LateReverbFilterCalculator::SubBandMessageQueue & lateReflectionSubbandFilters )
{
  std::vector<objectmodel::ObjectId> foundReverbObjects;
  for( objectmodel::ObjectVector::value_type const & objEntry : objects )
  {
    objectmodel::Object const & obj = *(objEntry.second);
    if( obj.numberOfChannels() != 1 )
    {
      std::cerr << "ReverbParameterCalculator: Only monaural object types are supported at the moment." << std::endl;
      continue;
    }

    objectmodel::ObjectTypeId const ti = obj.type();
    // Process reverb objects, ignore others
    switch( ti )
    {
      case objectmodel::ObjectTypeId::PointSourceWithReverb:
      {
        foundReverbObjects.push_back( obj.id() );
      }
    }
  } //for( objectmodel::ObjectVector::value_type const & objEntry : objects )


  // Check the list of found reverb objects against the existing entries.
  if( foundReverbObjects.size() > mMaxNumberOfObjects )
  {
    throw std::runtime_error( "The number of reverb objects exceeds the maximum admissible number." );
  }
  mChannelAllocator->setObjects( foundReverbObjects );
  for( std::size_t chIdx( 0 ); chIdx < mMaxNumberOfObjects; ++chIdx )
  {
    objectmodel::ObjectId const objId = mChannelAllocator->getObjectForChannel( chIdx );
    if( objId == objectmodel::Object::cInvalidChannelIndex )
    {
      signalRouting.removeEntry( chIdx );
      clearSingleObject( chIdx, discreteReflGains, discreteReflGains, biquadCoeffs, discretePanningMatrix, lateReflectionSubbandFilters );
    }
    else
    {
      objectmodel::PointSourceWithReverb const & rsao = dynamic_cast<objectmodel::PointSourceWithReverb const &>(objects.at( objId ));
      signalRouting.addRouting( rsao.channelIndex(0), chIdx ); // This clear existing routings to this rendering channel.
      processSingleObject( rsao, chIdx, discreteReflGains, discreteReflGains, biquadCoeffs, discretePanningMatrix, lateReflectionSubbandFilters );
    }
  }
} //process


void ReverbParameterCalculator::processInternal(const objectmodel::ObjectVector & objects)
{
    std::cerr << "internal processing" << std::endl;
}

void ReverbParameterCalculator::processSingleObject( objectmodel::PointSourceWithReverb const & rsao,
                                                     std::size_t renderChannel,
                                                     efl::BasicVector<ril::SampleType> & discreteReflGains,
                                                     efl::BasicVector<ril::SampleType> & discreteReflDelays,
                                                     pml::BiquadParameterMatrix<ril::SampleType> & biquadCoeffs,
                                                     efl::BasicMatrix<ril::SampleType> & discretePanningMatrix,
                                                     LateReverbFilterCalculator::SubBandMessageQueue & lateReflectionSubbandFilters )
{
  // TODO: Assign the biquad coefficients for the direct path.

  if( rsao.numberOfDiscreteReflections() > mNumberOfDiscreteReflectionsPerSource )
  {
    throw std::invalid_argument( "ReverbParameterCalculator::process(): Number of discrete reflections exceed maximum." );
  }
 
  // Define a starting index into the all parameter matrices 
  std::size_t const startRow = renderChannel * mNumberOfDiscreteReflectionsPerSource;

  // Assign the positions for the discrete reflections.
  for( std::size_t srcIdx( 0 ); srcIdx < rsao.numberOfDiscreteReflections( ); ++srcIdx )
  {
    objectmodel::PointSourceWithReverb::DiscreteReflection const & discRefl = rsao.discreteReflection( srcIdx );
    std::size_t matrixRow = startRow + srcIdx;

    // Set the position for the VBAP calculator
    mSourcePositions[srcIdx] = panning::XYZ( discRefl.positionX( ), discRefl.positionY( ), discRefl.positionZ( ) );

    // Set the biquad filters
    for( std::size_t biquadIdx( 0 ); biquadIdx < mNumberOfDiscreteReflectionsPerSource; ++biquadIdx )
    {
      biquadCoeffs( matrixRow, biquadIdx ) = discRefl.reflectionFilter( biquadIdx );
    }

    // Set the gain and delay for each discrete reflection
    discreteReflGains[matrixRow] = rsao.level() * discRefl.level();
    discreteReflDelays[matrixRow] = discRefl.delay();
  }

  // Fill the remaining discrete reflections with neutral values.
  static const panning::XYZ defaultPosition( 1.0f, 0.0f, 0.0f );
  static const pml::BiquadParameter<ril::SampleType> defaultBiquad; // Neutral flat biquad (default constructed)
  for( std::size_t srcIdx( rsao.numberOfDiscreteReflections() ); srcIdx < mNumberOfDiscreteReflectionsPerSource; ++srcIdx )
  {
    std::size_t const matrixRow = startRow + srcIdx;
    mSourcePositions[srcIdx] = defaultPosition;
    for( std::size_t biquadIdx( 0 ); biquadIdx < mNumberOfDiscreteReflectionsPerSource; ++biquadIdx )
    {
      biquadCoeffs( matrixRow, biquadIdx ) = defaultBiquad;
    }
    discreteReflGains[matrixRow] = 0.0f;
    discreteReflDelays[matrixRow] = 0.0f;

    // Already zero the panning gains for unused reflections
    // TODO: Control alignment via an option.
    if( efl::vectorZero( discretePanningMatrix.row( matrixRow ), mNumberOfPanningLoudspeakers,
                         0 /*Cannot make assumptions about alignment*/ ) != efl::noError )
    {
      throw std::runtime_error( "ReverbParameterCalculator: Error zeroing panning matrix for unused entries." );
    }
  }
  std::fill( mSourcePositions.begin() + rsao.numberOfDiscreteReflections(), mSourcePositions.end(), defaultPosition );
  if( mVbapCalculator.calcGains( ) != 0 )
  {
    std::cout << "ReverbParameterCalculator: Error calculating VBAP gains for discrete reflections." << std::endl;
  }

  for( std::size_t srcIdx( 0 ); srcIdx < rsao.numberOfDiscreteReflections(); ++srcIdx )
  {
    Afloat const * const gainRow = mVbapCalculator.getGains( ).row( srcIdx );
    for( std::size_t lspIdx(0 ); lspIdx < mNumberOfPanningLoudspeakers; ++lspIdx )
    discretePanningMatrix( lspIdx, srcIdx ) = gainRow[lspIdx];
  }
  // The unused rows are already zeroed.

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Late reflection part.


}

void ReverbParameterCalculator::clearSingleObject( std::size_t renderChannel,
                                                   efl::BasicVector<ril::SampleType> & discreteReflGains,
                                                   efl::BasicVector<ril::SampleType> & discreteReflDelays,
                                                   pml::BiquadParameterMatrix<ril::SampleType> & biquadCoeffs,
                                                   efl::BasicMatrix<ril::SampleType> & discretePanningMatrix,
                                                   LateReverbFilterCalculator::SubBandMessageQueue & lateReflectionSubbandFilters )
{
  // Define a starting index into the all parameter matrices 
  std::size_t const startRow = renderChannel * mNumberOfDiscreteReflectionsPerSource;

  static const panning::XYZ defaultPosition( 1.0f, 0.0f, 0.0f );
  static const pml::BiquadParameter<ril::SampleType> defaultBiquad; // Neutral flat biquad (default constructed)
  std::fill( mSourcePositions.begin( ), mSourcePositions.end( ), defaultPosition );
  for( std::size_t srcIdx( 0 ); srcIdx < mNumberOfDiscreteReflectionsPerSource; ++srcIdx )
  {
    std::size_t const matrixRow = startRow + srcIdx;
    for( std::size_t biquadIdx( 0 ); biquadIdx < mNumberOfDiscreteReflectionsPerSource; ++biquadIdx )
    {
      biquadCoeffs( matrixRow, biquadIdx ) = defaultBiquad;
    }
    discreteReflGains[matrixRow] = 0.0f;
    discreteReflDelays[matrixRow] = 0.0f;
  }
}

} // namespace rcl
} // namespace visr
