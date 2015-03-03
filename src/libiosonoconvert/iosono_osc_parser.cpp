/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "iosono_osc_parser.hpp"

#include <libefl/cartesian_spherical_conversion.hpp>
#include <libefl/degree_radian_conversion.hpp>

#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/object_factory.hpp>

#include <libobjectmodel/diffuse_source.hpp>
#include <libobjectmodel/point_source.hpp>
#include <libobjectmodel/point_source_with_diffuseness.hpp>
#include <libobjectmodel/plane_wave.hpp>

// avoid annoying warning about unsafe standard library functions in oscpkt.hh.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif
#include <oscpkt.hh>

#include <ciso646>
#include <cstdint>
#include <iostream>

namespace visr
{
namespace convertiosono
{

IosonoOscParser::IosonoOscParser()
 : IosonoOscParser( 30.0f )
{
}

IosonoOscParser::IosonoOscParser( float panningElevationDegree )
 : mOscParser( new oscpkt::PacketReader() )
 , cPanningElevation( efl::degree2radian( panningElevationDegree ) )
{
}

IosonoOscParser::~IosonoOscParser()
{
}
  
void IosonoOscParser::parse( char const * oscMessageData,
                             std::size_t size,
                             objectmodel::ObjectVector & parsedObjects )
{
  mOscParser->init( oscMessageData, size );

  oscpkt::Message* msg;
  while( (msg = mOscParser->popMessage()) != nullptr )
  {
    Packet currentPacket;
    bool parseResult = parseObjectMessage( *msg, currentPacket );
    if( !parseResult )
    {
      continue;
    }
    writeToObjectVector( currentPacket, parsedObjects );
  }
}

bool IosonoOscParser::parseObjectMessage( oscpkt::Message const & msg, Packet& packet )
{
  if( msg.match( "/iosono/renderer/version1/src" ) )
  {
    packet.protocol = Protocol::ThreeD;
  }
  else if( msg.match( "/iosono/renderer/verticalpan/v1/src" ) )
  {
    packet.protocol = Protocol::VerticalPan;
  }
  else
  {
    std::cerr << "IosonoOscParser: Received non-matching OSC packet." << std::endl;
    return false;
  }
  int32_t algorithmHint;
  if( !msg.arg()
    .popInt32( packet.channelNumber )
    .popInt32( algorithmHint )
    .popFloat( packet.sourceAzimuth )
    .popFloat( packet.sourceElevationOrHeightPanning )
    .popFloat( packet.sourceRadius )
    .popFloat( packet.sourceVolume )
    .popFloat( packet.sourceLfeVolume )
    .popFloat( packet.sourceDelay )
    .popInt32( packet.sourceScaling )
    .popInt32( packet.sourceOnScreen )
    .popFloat( packet.sourceSpreading )
    .popInt32( packet.sourceTraitFlags )
    .isOkNoMoreArgs() )
  {
    std::cerr << "IosonoOscParser: Parsing of OSC packet failed." << std::endl;
    return false;
  }
  bool const isPW = ((algorithmHint bitand (1 << 0)) != 0);
  packet.sourceType = isPW ? SourceType::PlaneWave : SourceType::PointSource;
  return true;
}

void IosonoOscParser::writeToObjectVector( Packet const & packet, objectmodel::ObjectVector & objVec )
{
  using namespace objectmodel;

  ObjectTypeId typeId;
  // Choice: The source spread/ diffusseness property takes precedence:
  // Even if the algorithm hint says 'plane wave', a source is rendered as a
  // diffuse source or diffuse pount source if the source spread is > 0.0
  if( packet.sourceSpreading >= 0.0f )
  {
    if( packet.sourceSpreading >= 100.0f )
    {
      typeId = ObjectTypeId::DiffuseSource;
    }
    else
    {
      typeId = ObjectTypeId::PointSourceWithDiffuseness;
    }
  }
  else
  {
    typeId = packet.sourceType == SourceType::PlaneWave
     ? ObjectTypeId::PlaneWave
     : ObjectTypeId::PointSource;
  }

  std::unique_ptr<Object> newObj = ObjectFactory::create( typeId );

  if( packet.channelNumber < 0 )
  {
    throw std::invalid_argument( "IosonoOscParser:: The channel number, i.e., the object id, must be greater or equal than zero.");
  }
  ObjectId const id = packet.channelNumber;
  newObj->setObjectId( id );
  newObj->resetNumberOfChannels( 1 ); // only single-channel objects are supported by the IOSONO protocol
  newObj->setChannelIndex( 0, id );   // the object id always matches the channel number
  newObj->setGroupId( 0 );            // Group ids are not provided by the IOSONO protocol
  newObj->setPriority( 0 );           // No priorities are set by the IOSONO protocol.
  newObj->setLevel( packet.sourceVolume );
  // Note: the objectmodel format has no support for separate LFE volumes. Therefore this attribute is discarded here.
 
  switch( typeId )
  {
  case ObjectTypeId::PointSourceWithDiffuseness:
  {
    PointSourceWithDiffuseness & psdObj = dynamic_cast<PointSourceWithDiffuseness &>(*newObj);
    psdObj.setDiffuseness( 1.0f * static_cast<Object::Coordinate>(packet.sourceSpreading) );

    // fall through intentionally
  }
  case ObjectTypeId::PointSource:
  {
    PointSource & psObj = dynamic_cast<PointSource &>(*newObj);

    Object::Coordinate const el = packet.protocol == Protocol::ThreeD
      ? packet.sourceElevationOrHeightPanning
      // In 'height panning mode', determine the object elevation by scaling the fixed elevation of the 
      // elevated loudspeaker ring with the transmitted panning factor.
      : packet.sourceElevationOrHeightPanning * cPanningElevation;
    Object::Coordinate x, y, z;

    std::tie( x, y, z ) = efl::spherical2cartesian( packet.sourceAzimuth, el,
                                                    packet.sourceRadius );
    psObj.setX( x );
    psObj.setX( y );
    psObj.setZ( z );
  }
  case ObjectTypeId::PlaneWave:
  {
    PlaneWave & pwObj = dynamic_cast<PlaneWave &>(*newObj);
    pwObj.setIncidenceAzimuth( efl::radian2degree( packet.sourceAzimuth ) );
    Object::Coordinate const el = packet.protocol == Protocol::ThreeD
      ? packet.sourceElevationOrHeightPanning
      // In 'height panning mode', determine the object elevation by scaling the fixed elevation of the 
      // elevated loudspeaker ring with the transmitted panning factor.
      : packet.sourceElevationOrHeightPanning * cPanningElevation;
    pwObj.setIncidenceElevation( efl::radian2degree( el ) );
    break;
  }
  default:
    assert( "IosonoOscParser: Unknown source type set.");
  }

  objVec.set( id, *newObj );
}

  
} // namespace convertiosono
} // namespace visr
