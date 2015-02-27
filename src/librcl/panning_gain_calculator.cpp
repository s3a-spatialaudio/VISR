/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "panning_gain_calculator.hpp"

#include <libefl/basic_matrix.hpp>
#include <libefl/vector_functions.hpp>

#include <libobjectmodel/object_vector.hpp>
// Note: At the moment, all supported source types are translated directly in this file
// TODO: For the future, consider moving this to another location.
#include <libobjectmodel/point_source.hpp>
#include <libobjectmodel/plane_wave.hpp>

#include <libpml/listener_position.hpp>

#include <boost/filesystem.hpp>

#include <ciso646>
#include <cstdio>
#include <tuple>

// for math utility functions (see implementations in the unnamed namespace below)
#include <boost/math/constants/constants.hpp>
#include <cmath>

namespace visr
{
namespace rcl
{

namespace // unnamed namespace
{
/**
 * Collection of mathematic utility functions
 * @todo Consider moving to a common location (math utility library?).
 */
//@{
template< typename T >
T degree2radian( T deg )
{
  return boost::math::constants::degree<T>() * deg;
}

template< typename T >
T radian2degree( T rad )
{
  return boost::math::constants::radian<T>() * rad;
}

template< typename T >
std::tuple< T, T, T > spherical2cartesian( T az, T el, T radius )
{
  return std::make_tuple( std::cos(az)*std::cos( el ) * radius,
                          std::sin(az)*std::cos( el ) * radius,
                          std::sin( el ) * radius );
};
//@}
} // unnamed namespace

PanningGainCalculator::PanningGainCalculator( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
 , mNumberOfObjects( 0 )
{
}

PanningGainCalculator::~PanningGainCalculator()
{
}

void PanningGainCalculator::setup( std::size_t numberOfObjects, std::size_t numberOfLoudspeakers, std::string const & arrayConfigFile )
{
  mNumberOfObjects = numberOfObjects;
  mNumberOfLoudspeakers = numberOfLoudspeakers;

  boost::filesystem::path const filePath = absolute( boost::filesystem::path(arrayConfigFile) );

  FILE* rawHandle = fopen( filePath.string().c_str(), "r" );
  if( rawHandle == 0 )
  {
    throw std::invalid_argument( std::string("PanningGainCalculator::setup(): Cannot open loudspeaker configuration file: ") + strerror(errno) + "." );
  }
  if( mSpeakerArray.load( rawHandle ) != 0 )
  {
    fclose( rawHandle );
    throw std::invalid_argument( "PanningGainCalculator::setup(): Error parsing loudspeaker configuration file." );
  }
  fclose( rawHandle );

  if( mNumberOfLoudspeakers != static_cast<std::size_t>(mSpeakerArray.getNumSpeakers() ) )
  {
    throw std::invalid_argument( "PanningGainCalculator::setup() The loudspeaker configuration file does not match to the given number of loudspeaker channels." );
  }

  mVbapCalculator.setNumSources( mNumberOfObjects );
  mVbapCalculator.setLoudspeakerArray( &mSpeakerArray );
  // set the default initial listener position. This also initialises the internal data members (e.g. inverse matrices)
  setListenerPosition( static_cast<CoefficientType>(0.0), static_cast<CoefficientType>(0.0), static_cast<CoefficientType>(0.0) );
  mLevels.resize( mNumberOfObjects );
  mLevels = 0.0f;
}

void PanningGainCalculator::setListenerPosition( CoefficientType x, CoefficientType y, CoefficientType z )
{
  mVbapCalculator.setListenerPosition( x, y, z );
  if( mVbapCalculator.calcInvMatrices() != 0 )
  {
    throw std::invalid_argument( "PanningGainCalculator::setup(): Calculation of inverse matrices failed." );
  }
}

void PanningGainCalculator::setListenerPosition( pml::ListenerPosition const & pos )
{
  setListenerPosition( pos.x(), pos.y(), pos.z() );
}

void PanningGainCalculator::process( objectmodel::ObjectVector const & objects, efl::BasicMatrix<CoefficientType> & gainMatrix )
{
  if( (gainMatrix.numberOfRows() != mNumberOfLoudspeakers) or (gainMatrix.numberOfColumns() != mNumberOfObjects) )
  {
    throw std::invalid_argument( "PanningGainCalculator::process(): The size of the gain matrix does not match the object/loudspeaker configuration." );
  }
  gainMatrix.zeroFill();

  mLevels = 0.0f;
  // As not every source object in the VBAP calculator component might correspond to a real source, we have to set them to safe position beforehand.
  XYZ defaultSource( 1.0f, 0.0f, 0.0f );
  for( std::size_t srcIdx(0); srcIdx < mNumberOfObjects; ++srcIdx )
  {
    mSourcePositions[ srcIdx ] = defaultSource;
  }

  // For the moment, we assume that the audio channels of the objects are identical to the final channel numbers.
  // Any potential re-routing will be added later.
  for( objectmodel::ObjectVector::value_type const & objEntry : objects )
  {
    objectmodel::Object const & obj = *(objEntry.second);
    if( obj.numberOfChannels() != 1 )
    {
      std::cerr << "PanningGainCalculator: Only monaural object types are supported at the moment." << std::endl;
      continue;
    }

    objectmodel::Object::ChannelIndex const channelId = obj.channelIndex( 0 );
    if( channelId >= mNumberOfObjects )
    {
      std::cerr << "PanningGainCalculator: Channel index \"" << channelId << "\" of object id#" << objEntry.first
                << "exceeds number of channels (" << mNumberOfObjects << ")." << std::endl;
      continue;
    }

    objectmodel::ObjectTypeId const ti = obj.type();

    // For the moment, we treat the two supported source type here.
    // @todo find a proper abstraction to handle many source types.
    switch( ti )
    {
      case objectmodel::ObjectTypeId::PointSource:
      {
        objectmodel::PointSource const & pointSrc = dynamic_cast<objectmodel::PointSource const &>(obj);
        mSourcePositions[ channelId ].set( pointSrc.x(), pointSrc.y(), pointSrc.z() );
        break;
      }
      case objectmodel::ObjectTypeId::PlaneWave:
      {
        objectmodel::PlaneWave const & planeSrc = dynamic_cast<objectmodel::PlaneWave const &>(obj);
        objectmodel::Object::Coordinate xPos, yPos, zPos;
        std::tie( xPos, yPos, zPos ) = spherical2cartesian( degree2radian(planeSrc.incidenceAzimuth()),
                                                            degree2radian(planeSrc.incidenceElevation()),
                                                            1.0f);
        mSourcePositions[ channelId ].set( xPos, yPos, zPos, true /*atInfinity corresponds to a plane wave */);
        break;
      }
      default:
        std::cerr << "PanningGainCalculator: Unsupported object type." << std::endl;
    }
    mLevels[ channelId ] = obj.level();

  } // for( objectmodel::ObjectVector::value_type const & objEntry : objects )
  mVbapCalculator.setSourcePositions( &mSourcePositions );
  if( mVbapCalculator.calcGains() != 0 )
  {
    throw std::runtime_error( "PanningGainCalculator: Error calculating VBAP gains." );
  }

  for( std::size_t chIdx(0); chIdx < mNumberOfObjects; ++chIdx )
  {
    Afloat const * const gainRow = (*mVbapCalculator.getGains())[ chIdx ];
    objectmodel::LevelType const level = mLevels[ chIdx ];
    for( std::size_t outIdx(0); outIdx < mNumberOfLoudspeakers; ++outIdx )
    {
      gainMatrix( outIdx, chIdx ) = level * gainRow[ outIdx ];
    }
  }
}

} // namespace rcl
} // namespace visr
