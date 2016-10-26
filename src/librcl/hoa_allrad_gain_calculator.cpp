/* Copyright Institute of Sound and Vibration Research - All rights reserved */

// avoid annoying warning about unsafe STL functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

#include "hoa_allrad_gain_calculator.hpp"

#include <libefl/basic_matrix.hpp>
#include <libefl/vector_functions.hpp>

#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/hoa_source.hpp>

#include <boost/filesystem.hpp>

#include <cassert>
#include <ciso646>
#include <cmath>
#include <cstdio>

namespace visr
{
namespace rcl
{

HoaAllRadGainCalculator::HoaAllRadGainCalculator( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
{
}

HoaAllRadGainCalculator::~HoaAllRadGainCalculator()
{
}

void HoaAllRadGainCalculator::setup( panning::LoudspeakerArray const & regularArrayConfig,
                                     panning::LoudspeakerArray const & realArrayConfig,
                                     efl::BasicMatrix<Afloat> const & decodeMatrix,
                                     pml::ListenerPosition const & listenerPosition /*= pml::ListenerPosition()*/ )
{
  mRegularSpeakerArray = regularArrayConfig;
  mRealSpeakerArray = realArrayConfig;

  std::size_t const numRegularSpeakers = mRegularSpeakerArray.getNumSpeakers( );
  std::size_t const numHarmonicSignals = decodeMatrix.numberOfRows();
  // Deduce the Ambisonics order from the number of harmonic signals
  std::size_t const hoaOrder = static_cast<std::size_t>(std::ceil( std::sqrt( numHarmonicSignals ) ))-1;
  if( ((hoaOrder + 1)*(hoaOrder + 1) != numHarmonicSignals) or (numHarmonicSignals == 0) )
  {
    throw std::invalid_argument( "The number of rows in the decoder matrix does not correspond to a real-valued HOA order." );
  }
  if( decodeMatrix.numberOfColumns() != numRegularSpeakers )
  {
    throw std::invalid_argument( "The number of columns in the decoder matrix does not match the size of the regular array." );
  }

  mRegularDecodeMatrix.resize( numHarmonicSignals, numRegularSpeakers );
  mRegularDecodeMatrix.copy( decodeMatrix );

  mVbapCalculator.setLoudspeakerArray( &mRealSpeakerArray );
  mVbapCalculator.setNumSources( static_cast<int>(numRegularSpeakers) );

  mAllRadCalculator.reset( new panning::AllRAD(&mRegularSpeakerArray, mRegularDecodeMatrix, static_cast<int>(hoaOrder) ) );

  // set the default initial listener position. 
  // This also initialises the internal data members (e.g., the VBAP calculator and the calculation of the VBAP decode matrix)
  setListenerPosition( listenerPosition );
}

void HoaAllRadGainCalculator::setListenerPosition( CoefficientType x, CoefficientType y, CoefficientType z )
{
  mVbapCalculator.setListenerPosition( x, y, z );
  precalculate();
}

void HoaAllRadGainCalculator::setListenerPosition( pml::ListenerPosition const & pos )
{
  setListenerPosition( pos.x(), pos.y(), pos.z() );
}

void HoaAllRadGainCalculator::precalculate()
{
  if( mVbapCalculator.calcInvMatrices( ) != 0 )
  {
    throw std::invalid_argument( "HoaAllRadGainCalculator::setup(): Calculation of inverse matrices failed." );
  }
  mAllRadCalculator->calcDecodeGains( &mVbapCalculator );
}

void HoaAllRadGainCalculator::process( objectmodel::ObjectVector const & objects, efl::BasicMatrix<CoefficientType> & gainMatrix )
{
  std::size_t const numRealSpeakers = gainMatrix.numberOfRows();
  std::size_t const numChannels = gainMatrix.numberOfColumns();

  if( numRealSpeakers != mRealSpeakerArray.getNumRegularSpeakers() )
  {
    throw std::invalid_argument( "HoaAllRadGainCalculator::process(): The size of the gain matrix does not match the number of real loudspeakers." );
  }
  // Do not reset the gain matrix, as we will write on top of the standard VBAP source gains.

  efl::BasicMatrix<Afloat> const & decodeGains = mAllRadCalculator->getDecodeGains();
  // NOTE: Currently, the gain matrix returned by the AllRAD object
  // also contains the gains of the imaginary speakers.
  // TODO: Reconsider the design of the VBAP objects to hide these
  // speakers from the outside.
  assert( decodeGains.numberOfRows() >= numRealSpeakers );

  // For the moment, we assume that the audio channels of the objects are identical to the final channel numbers.
  // Any potential re-routing will be added later.
  for( objectmodel::ObjectVector::value_type const & objEntry : objects )
  {
    objectmodel::Object const & obj = *(objEntry.second);

    // Note: This does not allow object types derived from HoaSource.
    objectmodel::ObjectTypeId const ti = obj.type( );
    if( ti != objectmodel::ObjectTypeId::HoaSource )
    {
      continue;
    }
    try
    {
      objectmodel::HoaSource const & hoaSrc = dynamic_cast<objectmodel::HoaSource const &>(obj);
      objectmodel::LevelType const srcLevel = hoaSrc.level( );
      std::size_t const numHoaSignals = hoaSrc.numberOfChannels( );
      if( numHoaSignals != (hoaSrc.order()+1)*(hoaSrc.order()+1) )
      {
        throw std::runtime_error( "The number of harmonic signals is inconsistent with the HOA order." );
      }
      for( std::size_t sigIdx( 0 ); sigIdx < numHoaSignals; ++sigIdx )
      {
        std::size_t const chIdx = hoaSrc.channelIndex( sigIdx );
        if( chIdx >= numChannels )
        {
          throw std::runtime_error( "Channel index exceeds maximum admissible value." );
        }
        for( std::size_t spkIdx(0); spkIdx < numRealSpeakers; ++spkIdx )
        {
          gainMatrix(spkIdx, chIdx ) = srcLevel * decodeGains( sigIdx, spkIdx );
        }
      }
    }
    catch( std::exception const & ex )
    {
      std::cout << "Error decoding HOA source: " << ex.what() << std::endl;
    }
  } // for( objectmodel::ObjectVector::value_type const & objEntry : objects )
}

} // namespace rcl
} // namespace visr
