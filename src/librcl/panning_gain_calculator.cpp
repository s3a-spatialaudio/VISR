/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "panning_gain_calculator.hpp"

#include <libefl/basic_matrix.hpp>

#include <libobjectmodel/object_vector.hpp>

#include <boost/filesystem.hpp>

#include <ciso646>
#include <cstdio>

namespace visr
{
namespace rcl
{

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
  // set default listener position
  mVbapCalculator.setListenerPosition( static_cast<Afloat>(0.0), static_cast<Afloat>(0.0), static_cast<Afloat>(0.0) );
  if( mVbapCalculator.calcInvMatrices() != 0 )
  {
    throw std::invalid_argument( "PanningGainCalculator::setup(): Calculation of inverse matrices failed." );
  }
}

void PanningGainCalculator::process( objectmodel::ObjectVector const & objects, efl::BasicMatrix<CoefficientType> & gainMatrix )
{
  if( (gainMatrix.numberOfRows() != mNumberOfLoudspeakers) or (gainMatrix.numberOfColumns() != mNumberOfObjects) )
  {
    throw std::invalid_argument( "PanningGainCalculator::process(): The size of the gain matrix does not match the object/loudspeaker configuration." );
  }
  gainMatrix.zeroFill();

  // For the moment, we assume that the audio channels of the objects are identical to the final channel numbers.
  // Any potential re-routing will be added later.
  for( objectmodel::ObjectVector::value_type const & objEntry : objects )
  {
    objectmodel::Object const & obj = *(objEntry.second);
    if( obj.numberOfChannels() != 1 )
    {
      std::cerr << "PanningGainCalculator: Only monaural object types are supported at the moment." << std::endl;
    }

    objectmodel::Object::ChannelIndex const channelId = obj.channelIndex( 0 );
    objectmodel::ObjectTypeId const typeId = obj.type();

    // XYZ const srcPos =
  }
}

} // namespace rcl
} // namespace visr
