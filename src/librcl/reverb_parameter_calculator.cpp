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

#include <libpml/message_queue.hpp>
#include <libpml/signal_routing_parameter.hpp>

#include <librbbl/object_channel_allocator.hpp>

#include <algorithm>
#include <cassert>
#include <ciso646>
#include <cmath>
#include <limits>
#include <numeric>
#include <vector>

namespace visr
{
namespace rcl
{


namespace
{

/**
 * Local helper function to find the maximum difference between corresponding elements of
 * two sequences of equal length
 */
ril::SampleType maxDiff( objectmodel::PointSourceWithReverb::LateReverbCoeffs const & lhs,
                         objectmodel::PointSourceWithReverb::LateReverbCoeffs const & rhs )
{
  ril::SampleType const res = std::inner_product(lhs.begin(), lhs.end(), rhs.begin(), 0.0f,
                            [](ril::SampleType v1, ril::SampleType v2) { return std::max( v1, v2 ); },
                            [](ril::SampleType v1, ril::SampleType v2) { return std::abs( v1 - v2 ); } );
  return res;
}

/**
 * Local helper function to compare two LateReverb objects up to a given tolerance value.
 */
bool equal( objectmodel::PointSourceWithReverb::LateReverb const & lhs,
            objectmodel::PointSourceWithReverb::LateReverb const & rhs,
            ril::SampleType limit )
{
  return (std::abs( lhs.onsetDelay() - rhs.onsetDelay() ) <= limit )
   and (maxDiff( lhs.levels(), rhs.levels()) <= limit )
   and (maxDiff( lhs.decayCoeffs(), rhs.decayCoeffs()) <= limit )
   and (maxDiff( lhs.attackTimes(), rhs.attackTimes()) <= limit );
}

} // unnamed namespace

/*static*/ const objectmodel::PointSourceWithReverb::LateReverb
ReverbParameterCalculator::cDefaultLateReverbParameter( 0.0, {0.0f}, {0.0f}, { 0.0f });

  /**
   * A table holding the previous states of the reverb parameters for the reverb channel.
   * Used to detect changes in the that trigger an retransmission to the LateReverbFilterCalculator component.
   */
  std::vector<objectmodel::PointSourceWithReverb::LateReverb> mPreviousLateReverbs;


ReverbParameterCalculator::ReverbParameterCalculator( ril::AudioSignalFlow& container, char const * name )
 : AtomicComponent( container, name )
 , mMaxNumberOfObjects( 0 )
 , mObjectInput( *this, "objectInput", pml::EmptyParameterConfig() )
 , cLateReverbParameterComparisonLimit( std::numeric_limits<ril::SampleType>::epsilon( ) )
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

    if( mNumberOfBiquadSectionsReflectionFilters != objectmodel::PointSourceWithReverb::cNumDiscreteReflectionBiquads )
    {
      throw std::invalid_argument( "The number of biquad sections for the discrete reflections differs from the constant used in the object definition." );
    }

    mChannelAllocator.reset( new rbbl::ObjectChannelAllocator( mMaxNumberOfObjects ) );

    // Configure the VBAP calculator
    mSourcePositions.resize( mNumberOfDiscreteReflectionsPerSource ); // Process one reverb object at a time.
    mVbapCalculator.setNumSources( mNumberOfDiscreteReflectionsPerSource /* * mMaxNumberOfObjects */ );
    mVbapCalculator.setSourcePositions(&mSourcePositions[0] );
    mVbapCalculator.setLoudspeakerArray( &arrayConfig );
    mVbapCalculator.setListenerPosition( 0.0f, 0.0f, 0.0f ); // Use a default listener position
    if( mVbapCalculator.calcInvMatrices( ) != 0 )
    {
      throw std::invalid_argument( "ReverbParameterCalculator::setup(): Calculation of inverse matrices for VBAP calculatorfailed." );
    }
    mNumberOfPanningLoudspeakers = mVbapCalculator.getNumSpeakers();

    mPreviousLateReverbs.resize( mMaxNumberOfObjects, cDefaultLateReverbParameter );
    // Set one member to an invalid value to trigger sending of late reverb messages on the first call to process();
    std::for_each(mPreviousLateReverbs.begin(), mPreviousLateReverbs.end(), []( objectmodel::PointSourceWithReverb::LateReverb & v ) { v.setOnsetDelay( -1.0f); } );

    // Set up the parameter ports.
    pml::VectorParameterConfig const discreteGainDelayConfig( mNumberOfDiscreteReflectionsPerSource * mMaxNumberOfObjects );
    pml::VectorParameterConfig const lateGainDelayConfig( mMaxNumberOfObjects );
    pml::MatrixParameterConfig const iirMatrixConfig( mNumberOfDiscreteReflectionsPerSource * mMaxNumberOfObjects, objectmodel::PointSourceWithReverb::cNumDiscreteReflectionBiquads );
    pml::MatrixParameterConfig const discretePanningConfig( mNumberOfPanningLoudspeakers, mNumberOfDiscreteReflectionsPerSource * mMaxNumberOfObjects );
    mSignalRoutingOutput.reset( new ril::ParameterOutputPort< pml::SharedDataProtocol, pml::SignalRoutingParameter >
      ( *this, "signalRoutingOutput", pml::EmptyParameterConfig() ) );
    mDiscreteReflectionGainOutput.reset( new ril::ParameterOutputPort < pml::SharedDataProtocol, pml::VectorParameter<ril::SampleType> >
      (*this, "discreteGainOutput", discreteGainDelayConfig ) );
    mDiscreteReflectionDelayOutput.reset( new ril::ParameterOutputPort < pml::SharedDataProtocol, pml::VectorParameter<ril::SampleType> >
      (*this, "discreteDelayOutput", discreteGainDelayConfig ) );
    mDiscreteReflectionFilterCoeffOutput.reset( new ril::ParameterOutputPort< pml::SharedDataProtocol, pml::BiquadParameterMatrix<ril::SampleType> >
      (*this, "discreteFilterCoeffsOutput", iirMatrixConfig ) );
    mDiscretePanningGains.reset( new ril::ParameterOutputPort< pml::SharedDataProtocol, pml::MatrixParameter<ril::SampleType> >
      (*this, "discretePanningMatrixOutput", discretePanningConfig) );
    mLateReflectionGainOutput.reset( new ril::ParameterOutputPort < pml::SharedDataProtocol, pml::VectorParameter<ril::SampleType> >
      ( *this, "lateGainOutput", lateGainDelayConfig ) );
    mLateReflectionDelayOutput.reset( new ril::ParameterOutputPort < pml::SharedDataProtocol, pml::VectorParameter<ril::SampleType> >
      ( *this, "lateDelayOutput", lateGainDelayConfig ) );
    mLateSubbandOutput.reset( new ril::ParameterOutputPort < pml::MessageQueueProtocol, pml::IndexedValueParameter< std::size_t, std::vector<ril::SampleType> > >
      (*this, "lateSubbandOutput", pml::EmptyParameterConfig( )) );
}

/**
* The process function.
* It takes a vector of objects as input and calculates a vector of output gains.
*/
void ReverbParameterCalculator::process()
{
  // Get and check the data members.
  pml::SignalRoutingParameter & signalRouting = mSignalRoutingOutput->data();
  efl::BasicVector<ril::SampleType> & discreteReflGains = mDiscreteReflectionGainOutput->data();
  efl::BasicVector<ril::SampleType> & discreteReflDelays = mDiscreteReflectionDelayOutput->data();
  pml::BiquadParameterMatrix<ril::SampleType> & biquadCoeffs = mDiscreteReflectionFilterCoeffOutput->data();
  efl::BasicMatrix<ril::SampleType> & discretePanningMatrix = mDiscretePanningGains->data();
  efl::BasicVector<ril::SampleType> & lateReverbGains = mLateReflectionGainOutput->data( );
  efl::BasicVector<ril::SampleType> & lateReverbDelays = mLateReflectionDelayOutput->data( );

  if( discreteReflGains.size() != mMaxNumberOfObjects )
  {
    throw std::invalid_argument( "ReverbParameterCalculator::process( ): output parameter of port "" has a wrong vector length." );
  }

  std::vector<objectmodel::ObjectId> foundReverbObjects;
  pml::ObjectVector const & objects = mObjectInput.data();
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
        foundReverbObjects.push_back( obj.id() );
        break;
      default: // Default handling to prevent compiler warnings about unhandled enumeration values
      {
        // Do nothing
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
      clearSingleObject( chIdx, discreteReflGains, discreteReflDelays, biquadCoeffs, discretePanningMatrix,
                         lateReverbGains, lateReverbDelays );
    }
    else
    {
      objectmodel::PointSourceWithReverb const & rsao = dynamic_cast<objectmodel::PointSourceWithReverb const &>(objects.at( objId ));
      signalRouting.addRouting( rsao.channelIndex(0), chIdx ); // This clear existing routings to this rendering channel.
      processSingleObject( rsao, chIdx, discreteReflGains, discreteReflDelays, biquadCoeffs, discretePanningMatrix, 
                           lateReverbGains, lateReverbDelays);
    }
  }
} //process

void ReverbParameterCalculator::processSingleObject( objectmodel::PointSourceWithReverb const & rsao,
                                                     std::size_t renderChannel,
                                                     efl::BasicVector<ril::SampleType> & discreteReflGains,
                                                     efl::BasicVector<ril::SampleType> & discreteReflDelays,
                                                     pml::BiquadParameterMatrix<ril::SampleType> & biquadCoeffs,
                                                     efl::BasicMatrix<ril::SampleType> & discretePanningMatrix,
                                                     efl::BasicVector<ril::SampleType> & lateReverbGains,
                                                     efl::BasicVector<ril::SampleType> & lateReverbDelays )
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
    std::size_t const matrixIdx = startRow + srcIdx;
    for( std::size_t lspIdx(0 ); lspIdx < mNumberOfPanningLoudspeakers; ++lspIdx )
    discretePanningMatrix( lspIdx, matrixIdx ) = gainRow[lspIdx];
  }
  // The unused rows are already zeroed.

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Late reflection part.
  lateReverbDelays[renderChannel] = 0.0f; // Unused for now.  We might apply the frequency-independent offset delay here.
  lateReverbGains[renderChannel] = rsao.level(); // Adjust to the object level.
  if( not equal( mPreviousLateReverbs[renderChannel], rsao.lateReverb(), cLateReverbParameterComparisonLimit ))
  {
    mPreviousLateReverbs[renderChannel] = rsao.lateReverb();
    // XXX lateReflectionSubbandFilters.enqueue( std::make_pair( renderChannel, mPreviousLateReverbs[renderChannel] ));
  }
}

void ReverbParameterCalculator::clearSingleObject( std::size_t renderChannel,
                                                   efl::BasicVector<ril::SampleType> & discreteReflGains,
                                                   efl::BasicVector<ril::SampleType> & discreteReflDelays,
                                                   pml::BiquadParameterMatrix<ril::SampleType> & biquadCoeffs,
                                                   efl::BasicMatrix<ril::SampleType> & discretePanningMatrix,
                                                   efl::BasicVector<ril::SampleType> & lateReverbGains,
                                                   efl::BasicVector<ril::SampleType> & lateReverbDelays )
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
  // Late reflection part
  lateReverbDelays[renderChannel] = 0.0f;
  lateReverbGains[renderChannel] = 0.0f;  

  if( not equal( mPreviousLateReverbs[renderChannel], cDefaultLateReverbParameter, cLateReverbParameterComparisonLimit ))
  {
    mPreviousLateReverbs[renderChannel] = cDefaultLateReverbParameter;
    // XXX lateReflectionSubbandFilters.enqueue( std::make_pair( renderChannel, mPreviousLateReverbs[renderChannel] ));
  }

}

} // namespace rcl
} // namespace visr
