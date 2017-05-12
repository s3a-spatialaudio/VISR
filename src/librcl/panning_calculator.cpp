/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "panning_calculator.hpp"

#include <libril/status_message.hpp>

#include <libefl/basic_matrix.hpp>
#include <libefl/cartesian_spherical_conversion.hpp>
#include <libefl/degree_radian_conversion.hpp>
#include <libefl/matrix_functions.hpp>
#include <libefl/vector_functions.hpp>

#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/diffuse_source.hpp>
#include <libobjectmodel/point_source.hpp>
#include <libobjectmodel/point_source_with_diffuseness.hpp>
#include <libobjectmodel/plane_wave.hpp>

#include <libpanning/LoudspeakerArray.h>

#include <libpml/listener_position.hpp>
#include <libpml/shared_data_protocol.hpp>
#include <libpml/matrix_parameter.hpp>
#include <libpml/object_vector.hpp>
#include <libpml/double_buffering_protocol.hpp>


#include <algorithm>
#include <ciso646>
#include <cmath>
#include <cstdio>
#include <iterator>
#include <numeric>

// for math utility functions (see implementations in the unnamed namespace below)

namespace visr
{
namespace rcl
{

  PanningCalculator::PanningCalculator( SignalFlowContext const & context,
                                        char const * name,
                                        CompositeComponent * parent /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mNumberOfObjects( 0 )
 , mLoudspeakerDotProducts( cVectorAlignmentSamples )
 , mHighFrequencyGains(cVectorAlignmentSamples )
{
}

PanningCalculator::~PanningCalculator()
{
}

void PanningCalculator::setup( std::size_t numberOfObjects,
                               panning::LoudspeakerArray const & arrayConfig,
                               bool adaptiveListenerPosition /*= false*/,
                               bool separateLowpassPanning /*= false*/ )
{
  mSpeakerArray = arrayConfig;
  mNumberOfObjects = numberOfObjects;
  mNumberOfRegularLoudspeakers = arrayConfig.getNumRegularSpeakers();
  mNumberOfAllLoudspeakers = arrayConfig.getNumSpeakers();

  mVectorDimension = arrayConfig.is2D() ? 2 : 3;
  // This contains all loudspeakers.
  mLoudspeakerPositions.resize( mVectorDimension, mNumberOfAllLoudspeakers );
  for( std::size_t lspIdx( 0 ); lspIdx < mNumberOfAllLoudspeakers; ++lspIdx )
  {
    panning::XYZ pos = arrayConfig.getPosition(lspIdx);
    pos.normalise();
    mLoudspeakerPositions( 0, lspIdx ) = pos.x;
    mLoudspeakerPositions( 1, lspIdx ) = pos.y;
    if( mVectorDimension > 2 )
    {
      mLoudspeakerPositions( 2, lspIdx ) = pos.z;
    }
  }
  mSourceCoordinates.resize( mNumberOfObjects, mVectorDimension );

  // Compute the triplet centers
  efl::BasicMatrix<CoefficientType> tripletCenters(mVectorDimension, arrayConfig.getNumTriplets() );
  std::vector<bool> tripletProcessed( arrayConfig.getNumTriplets(), false );

  mLoudspeakerDotProducts.resize( mNumberOfRegularLoudspeakers );


  mVbapCalculator.reset( new panning::VBAP( mSpeakerArray ) );

  // set the default initial listener position. This also initialises the internal data members (e.g. inverse matrices)
  setListenerPosition( static_cast<CoefficientType>(0.0), static_cast<CoefficientType>(0.0), static_cast<CoefficientType>(0.0) );
  mLevels.resize( mNumberOfObjects );
  mLevels = 0.0f;

  mObjectVectorInput.reset( new ObjectPort( "objectVectorInput", *this, pml::EmptyParameterConfig() ) );
  mGainOutput.reset( new MatrixPort( "gainOutput", *this, pml::MatrixParameterConfig( mNumberOfRegularLoudspeakers, mNumberOfObjects ) ) );

  if( adaptiveListenerPosition )
  {
    mListenerPositionInput.reset( new ListenerPositionPort( "listenerPosition", *this, pml::EmptyParameterConfig() ) );
  }

  if( separateLowpassPanning )
  {
    mLowFrequencyGainOutput.reset( new MatrixPort( "lowFrequencyGainOutput", *this,
                                                  pml::MatrixParameterConfig( mNumberOfRegularLoudspeakers, mNumberOfObjects ) ));
    // What to do with virtual loudspeakers? At the moment, thei energy is discarded. Thus computing a normalisation factor afterwards without incorporating them could
    // result in a very low value, boosting the gains of the real loudspeakers.
    // TODO: Reconsider after another strategy has been selected for handling virtual loudspeakers (downmix matrices before normalisation?)
    mHighFrequencyGains.resize( mNumberOfAllLoudspeakers );
  }
}

void PanningCalculator::setListenerPosition( CoefficientType x, CoefficientType y, CoefficientType z )
{
  mVbapCalculator->setListenerPosition( x, y, z );
}

void PanningCalculator::setListenerPosition( pml::ListenerPosition const & pos )
{
  setListenerPosition( pos.x(), pos.y(), pos.z() );
}

void PanningCalculator::process()
{
  bool const listenerPosChanged = mListenerPositionInput && mListenerPositionInput->changed();
  if( listenerPosChanged )
  {
    setListenerPosition( mListenerPositionInput->data( ) );
    mListenerPositionInput->resetChanged( );
  }

  if( mObjectVectorInput->changed() )
  {
    objectmodel::ObjectVector const & objects = mObjectVectorInput->data( );

    pml::MatrixParameter<CoefficientType> & gainMatrix = mGainOutput->data( );

    if( (gainMatrix.numberOfRows( ) != mNumberOfRegularLoudspeakers) or( gainMatrix.numberOfColumns( ) != mNumberOfObjects ) )
    {
      throw std::invalid_argument( "PanningCalculator::process(): The size of the gain matrix does not match the object/loudspeaker configuration." );
    }
    // Not necessary if we opt to set the complete matrix.
    gainMatrix.zeroFill( );

    // TODO: The level is now applied elsewhere.
    mLevels = 0.0f;

    // For the moment, we assume that the audio channels of the objects are identical to the final channel numbers.
    // Any potential re-routing will be added later.
    for( objectmodel::ObjectVector::value_type const & objEntry : objects )
    {
      objectmodel::Object const & obj = *(objEntry.second);
      if( obj.numberOfChannels( ) != 1 )
      {
        std::cerr << "PanningCalculator: Only monaural object types are supported at the moment." << std::endl;
        continue;
      }

      objectmodel::Object::ChannelIndex const channelId = obj.channelIndex( 0 );
      if( channelId >= mNumberOfObjects )
      {
        std::cerr << "PanningCalculator: Channel index \"" << channelId << "\" of object id#" << objEntry.first
          << "exceeds number of channels (" << mNumberOfObjects << ")." << std::endl;
        continue;
      }

      mLevels[channelId] = obj.level( );

      objectmodel::ObjectTypeId const ti = obj.type( );

      // For the moment, we treat the two supported source type here.
      // @todo find a proper abstraction to handle many source types.
      switch( ti )
      {
      case objectmodel::ObjectTypeId::PointSourceWithDiffuseness:
      {
        objectmodel::PointSourceWithDiffuseness const & psdSrc = dynamic_cast<objectmodel::PointSourceWithDiffuseness const &>(obj);
        mLevels[channelId] *= (static_cast<objectmodel::LevelType>(1.0) - psdSrc.diffuseness( )); // Adjust the amount of direct sound according to the diffuseness
        // Fall through intentionally
      }
      case objectmodel::ObjectTypeId::PointSource:
      case objectmodel::ObjectTypeId::PointSourceExtent:
      case objectmodel::ObjectTypeId::PointSourceWithReverb: // TODO: This shows that the current model is not extensible, because it does not consider type hierarchies
      {
        objectmodel::PointSource const & pointSrc = dynamic_cast<objectmodel::PointSource const &>(obj);
        // mVbapCalculator->calcGain( gainMatrix.row( channelId ) );
        break;
      }
      case objectmodel::ObjectTypeId::PlaneWave:
      {
                                                 objectmodel::PlaneWave const & planeSrc = dynamic_cast<objectmodel::PlaneWave const &>(obj);
                                                 objectmodel::Object::Coordinate xPos, yPos, zPos;
                                                 std::tie( xPos, yPos, zPos ) = efl::spherical2cartesian( efl::degree2radian( planeSrc.incidenceAzimuth( ) ),
                                                   efl::degree2radian( planeSrc.incidenceElevation( ) ),
                                                   1.0f );
//        mVbapCalculator->
        break;
      }
      default:
        // Ignore unknown source types by setting them to a zero level).
        // That means that the VBAP gains will be calculated for the default position, but zeroed afterwards.
        mLevels[channelId] = static_cast<objectmodel::LevelType>(0.0f);
      }
    } // for( objectmodel::ObjectVector::value_type const & objEntry : objects )

#if 0
    // TODO: Can be replaced by a vector multiplication.
    for( std::size_t chIdx( 0 ); chIdx < mNumberOfObjects; ++chIdx )
    {
      // Duplicated, inefficient code due to the use of the legacy VBAP object.
      objectmodel::ObjectVector::const_iterator findIt = objects.find( chIdx);
      if( findIt != objects.end() )
      {
        objectmodel::PointSource const * psSrc = dynamic_cast<objectmodel::PointSource const *>(findIt->second.get() );
        if( psSrc )
        {

          efl::ErrorCode res = efl::product( mSourceCoordinates.row( chIdx ), mLoudspeakerPositions.data(), mLoudspeakerDotProducts.data(),
                                            1 /*numResultRows*/, mNumberOfRegularLoudspeakers /* numResultColumns */, mVectorDimension /*numOp1Columns*/,
                                            mSourceCoordinates.stride() /* op1RowStride */, 1 /*op1ColumnStride*/,
                                            mLoudspeakerPositions.stride() /*op2RowStride*/, 1 /*op2ColumnStride*/,
                                            1 /*resRowStride, not used, since  only one row. */, 1 /*resColumnStride*/ );
          if( res != efl::noError )
          {
            // TODO: Replace by error API call.
            throw std::runtime_error( "PanningCalculator: dot product for computing channel lock failed.");
          }
          // Search only across the regular loudspeakers
          // We search for the maximum, because we are using the dot product, i.e., the cosine of the angular distance.
          // In case of equidistant nearest speakers, the first is returned.
          CoefficientType const * maxDotProduct = std::max_element( mLoudspeakerDotProducts.data(), mLoudspeakerDotProducts.data() + mNumberOfRegularLoudspeakers );
          if( maxDotProduct - mLoudspeakerDotProducts.data() >= static_cast<std::ptrdiff_t>(mNumberOfRegularLoudspeakers) )
          {
            // TODO: Replace by error API call.
            throw std::runtime_error( "PanningCalculator: dot product for computing channel lock failed.");
          }
          CoefficientType const lockLimit = std::cos( efl::degree2radian(psSrc->channelLockDistance() ) );
          if( *maxDotProduct >= lockLimit )
          {
            std::size_t const lockLspIdx = static_cast<std::size_t>( maxDotProduct - mLoudspeakerDotProducts.data() );
            // Not necessary if we cleared the matrix before
            for( std::size_t outIdx( 0 ); outIdx < mNumberOfRegularLoudspeakers; ++outIdx )
            {
              gainMatrix( outIdx, chIdx ) = static_cast<CoefficientType>(0.0);
            }
            gainMatrix( lockLspIdx, chIdx ) = mLevels[chIdx];
          }
          // Otherwise fall through to copying the already computed VBAP gains

          Afloat const * const gainRow = mVbapCalculator.getGains( ).row( chIdx );
          objectmodel::LevelType const level = mLevels[chIdx];
          if( separateLowpassPanning() )
          {
            // Compute the unnormalised high-freuency gains.
            // Note the use of the virtual loudspeakers (see comment about the size of mHighFrequencyGains in setup() )
            std::transform( gainRow, gainRow + mNumberOfAllLoudspeakers, mHighFrequencyGains.data(),
                           []( CoefficientType val ){ return std::sqrt(val); } );
            // Compute the l_2 norm of the high-frequency gains
            // TODO: Consider replacing by a library function.
            CoefficientType const l2NormHF = std::sqrt(std::accumulate( mHighFrequencyGains.data(),
                                                                       mHighFrequencyGains.data() + mNumberOfAllLoudspeakers, static_cast<CoefficientType>(0.0),
                                                                       [](CoefficientType acc, CoefficientType val ){ return acc += std::abs(val)*std::abs(val); } ) );


            CoefficientType const l1NormLF = std::accumulate( gainRow, gainRow+mNumberOfRegularLoudspeakers, static_cast<CoefficientType>(0.0),
                                                              [](CoefficientType acc, CoefficientType val ){ return acc += std::abs(val); } );

            pml::MatrixParameter<CoefficientType> & lfGainMatrix = mLowFrequencyGainOutput->data( );
            CoefficientType const scaleFactorLF = static_cast<CoefficientType>(1.0)/l1NormLF;
            CoefficientType const scaleFactorHF = static_cast<CoefficientType>(1.0)/l2NormHF;
            for( std::size_t outIdx( 0 ); outIdx < mNumberOfRegularLoudspeakers; ++outIdx )
            {
              gainMatrix( outIdx, chIdx ) = level * scaleFactorHF * mHighFrequencyGains[outIdx];
              lfGainMatrix( outIdx, chIdx ) = level * scaleFactorLF * gainRow[outIdx];
            }
          }
          else
          {
            // Standard VBAP choice. Use LF coefficients subject to a HF (energy) optimisation for everything.
            for( std::size_t outIdx( 0 ); outIdx < mNumberOfRegularLoudspeakers; ++outIdx )
            {
              gainMatrix( outIdx, chIdx ) = level * gainRow[outIdx];
            }
          }
        } // if( psSrc )
      }
      else
      {
        for( std::size_t outIdx( 0 ); outIdx < mNumberOfRegularLoudspeakers; ++outIdx )
        {
          gainMatrix( outIdx, chIdx ) = static_cast<CoefficientType>(0.0);
        }
      } // else branch of findIt != objects.end() )
    } // for
#endif
    mObjectVectorInput->resetChanged();
    }
}

} // namespace rcl
} // namespace visr
