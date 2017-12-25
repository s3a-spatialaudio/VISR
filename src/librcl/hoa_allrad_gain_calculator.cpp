/* Copyright Institute of Sound and Vibration Research - All rights reserved */

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

HoaAllRadGainCalculator::
HoaAllRadGainCalculator( SignalFlowContext const & context,
                         char const * name,
                         CompositeComponent * parent,
                         std::size_t numberOfObjectChannels,
                         panning::LoudspeakerArray const & regularArrayConfig,
                         panning::LoudspeakerArray const & realArrayConfig,
                         efl::BasicMatrix<Afloat> const & decodeMatrix,
                         pml::ListenerPosition const & listenerPosition /*= pml::ListenerPosition()*/,
                         bool adaptiveListenerPosition /*= false*/ )
  : AtomicComponent( context, name, parent )
  , mObjectInput( "objectInput", *this, pml::EmptyParameterConfig() )
  , mGainMatrixInput( "gainInput", *this, pml::MatrixParameterConfig( realArrayConfig.getNumRegularSpeakers(), numberOfObjectChannels ) )
  , mGainMatrixOutput( "gainOutput", *this, pml::MatrixParameterConfig( realArrayConfig.getNumRegularSpeakers(), numberOfObjectChannels ) )
  , mListenerInput( adaptiveListenerPosition ? new ParameterInput<pml::DoubleBufferingProtocol, pml::ListenerPosition>( "listenerInput", *this ) : nullptr )
  , mRegularDecodeMatrix( decodeMatrix.numberOfRows(), regularArrayConfig.getNumRegularSpeakers(), cVectorAlignmentSamples )
  , mRealDecodeMatrix( regularArrayConfig.getNumSpeakers(), realArrayConfig.getNumRegularSpeakers(), cVectorAlignmentSamples )
{
  std::size_t const numRegularSpeakers = regularArrayConfig.getNumSpeakers();
  std::size_t const numHarmonicSignals = decodeMatrix.numberOfRows();

  // Deduce the Ambisonics order from the number of harmonic signals
  std::size_t const hoaOrder = static_cast<std::size_t>(std::ceil( std::sqrt( numHarmonicSignals ) )) - 1;
  if( ((hoaOrder + 1)*(hoaOrder + 1) != numHarmonicSignals) or (numHarmonicSignals == 0) )
  {
    throw std::invalid_argument( "The number of rows in the decoder matrix does not correspond to a real-valued HOA order." );
  }
  if( decodeMatrix.numberOfColumns() != numRegularSpeakers )
  {
    throw std::invalid_argument( "The number of columns in the decoder matrix does not match the size of the regular array." );
  }

  mRegularDecodeMatrix.copy( decodeMatrix );

  mAllRadCalculator.reset( new panning::AllRAD( regularArrayConfig,
    realArrayConfig,
    mRegularDecodeMatrix,
    static_cast<unsigned int>(hoaOrder) ) );

  // set the default initial listener position. 
  // This also initialises the internal data members (e.g., the VBAP calculator and the calculation of the VBAP decode matrix)
  setListenerPosition( listenerPosition );
}

HoaAllRadGainCalculator::~HoaAllRadGainCalculator()
{
}

#if 0
void HoaAllRadGainCalculator::setup( std::size_t numberOfObjectChannels,
                                     panning::LoudspeakerArray const & regularArrayConfig,
                                     panning::LoudspeakerArray const & realArrayConfig,
                                     efl::BasicMatrix<Afloat> const & decodeMatrix,
                                     pml::ListenerPosition const & listenerPosition /*= pml::ListenerPosition()*/,
                                     bool adaptiveListenerPosition /*= false*/ )
{
  std::size_t const numRegularSpeakers = regularArrayConfig.getNumSpeakers( );
  std::size_t const numHarmonicSignals = decodeMatrix.numberOfRows();
  pml::MatrixParameterConfig const gainMtxConfig( realArrayConfig.getNumRegularSpeakers(), numberOfObjectChannels );
  mGainMatrixInput.setParameterConfig( gainMtxConfig );
  mGainMatrixOutput.setParameterConfig( gainMtxConfig );

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

  mAllRadCalculator.reset( new panning::AllRAD( regularArrayConfig,
                                                realArrayConfig,
                                                mRegularDecodeMatrix,
                                                static_cast<unsigned int>(hoaOrder) ) );


  mRealDecodeMatrix.resize( numHarmonicSignals, realArrayConfig.getNumRegularSpeakers() );


  // set the default initial listener position. 
  // This also initialises the internal data members (e.g., the VBAP calculator and the calculation of the VBAP decode matrix)
  setListenerPosition( listenerPosition );

}
#endif

void HoaAllRadGainCalculator::setListenerPosition( CoefficientType x, CoefficientType y, CoefficientType z )
{
  mAllRadCalculator->setListenerPosition( x, y, z );
}

void HoaAllRadGainCalculator::setListenerPosition( pml::ListenerPosition const & pos )
{
  setListenerPosition( pos.x(), pos.y(), pos.z() );
}

#if 0
// Not necessary anymore i
void HoaAllRadGainCalculator::precalculate()
{
#if 0
  if( mVbapCalculator.calcInvMatrices( ) != 0 )
  {
    throw std::invalid_argument( "HoaAllRadGainCalculator::setup(): Calculation of inverse matrices failed." );
  }
  mAllRadCalculator->calcDecodeGains( &mVbapCalculator );
#endif
}
#endif

void HoaAllRadGainCalculator::process()
{
  if( mListenerInput and mListenerInput->changed() )
  {
    pml::ListenerPosition const & pos = mListenerInput->data();
    mAllRadCalculator->setListenerPosition( pos.x(), pos.y(), pos.z() );
    mListenerInput->resetChanged();
  }

  // Because of the SharedData protocol of the gain input, we must always copy the input and
  // handle the HOA objects.
  pml::ObjectVector const & objects =  mObjectInput.data();
  pml::MatrixParameter<SampleType> const & inMtx = mGainMatrixInput.data();
  pml::MatrixParameter<SampleType> & outMtx = mGainMatrixOutput.data();
  outMtx.copy( inMtx );

  std::size_t const numChannels = outMtx.numberOfColumns();
  std::size_t const numLoudspeakers = outMtx.numberOfRows();

  efl::BasicMatrix<Afloat> const & decodeGains = mAllRadCalculator->decodingGains();

  // For the moment, we assume that the audio channels of the objects are identical to the final channel numbers.
  // Any potential re-routing will be added later.
  for( objectmodel::Object const & obj : objects )
  {
    // Note: This does not allow object types derived from HoaSource.
    objectmodel::ObjectTypeId const ti = obj.type();
    if( ti != objectmodel::ObjectTypeId::HoaSource )
    {
      continue;
    }
    try
    {
      objectmodel::HoaSource const & hoaSrc = dynamic_cast<objectmodel::HoaSource const &>(obj);
      std::size_t const numHoaSignals = hoaSrc.numberOfChannels();
      if( numHoaSignals != (hoaSrc.order() + 1)*(hoaSrc.order() + 1) )
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
        for( std::size_t spkIdx( 0 ); spkIdx < numLoudspeakers; ++spkIdx )
        {
          outMtx( spkIdx, chIdx ) = decodeGains( sigIdx, spkIdx );
        }
      }
    }
    catch( std::exception const & ex )
    {
      status( StatusMessage::Error, "Error decoding HOA source: ",  ex.what() );
      return;
    }
  } // for( objectmodel::ObjectVector::value_type const & objEntry : objects )
  mObjectInput.resetChanged(); // Always reset.
}

} // namespace rcl
} // namespace visr
