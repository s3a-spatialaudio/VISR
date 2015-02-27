/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "iosono_osc_parser.hpp"

#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/object_factory.hpp>

#include <libobjectmodel/diffuse_source.hpp>
#include <libobjectmodel/point_source.hpp>
#include <libobjectmodel/point_source_with_diffuseness.hpp>
#include <libobjectmodel/plane_wave.hpp>

// avoid annoying warning about unsafe standard library functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif
#include <oscpkt.hh>

#include <cstdint>
#include <iostream>

namespace visr
{
namespace convertiosono
{

  IosonoOscParser::IosonoOscParser()
  : mOscParser( new oscpkt::PacketReader() )
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
        bool parseResult = parseObjectMessage( msg, currentPacket );
        if( !parseResult )
        {
          continue;
        }
        Protocol pType;
        if( msg->match( "/iosono/renderer/version1/src" ) )
        {
          pType = Protocol::ThreeD;
        }
        else if( msg->match( "/iosono/renderer/verticalpan/v1/src" ) )
        {
          pType = Protocol::VerticalPan;
        }
        else
        {
          std::cerr << "IosonoOscParser: Received non-matching OSC packet." << std::endl;
          continue;
        }
        int32_t channelNumber;
        int32_t algorithmHint;
        float sourceAzimuth;
        float sourceElevationOrHeightPanning;
        float sourceRadius;
        float sourceVolume;
        float sourceLfeVolume;
        float sourceDelay;
        int32_t sourceScaling;
        int32_t sourceOnScreen;
        float sourceSpreading;
        int32_t sourceTraitFlags;

        bool const decodeResult = msg->arg()
          .popInt32( channelNumber )
          .popInt32( algorithmHint )
          .popFloat( sourceAzimuth )
          .popFloat( sourceElevationOrHeightPanning )
          .popFloat( sourceRadius )
          .popFloat( sourceVolume )
          .popFloat( sourceLfeVolume )
          .popFloat( sourceDelay )
          .popInt32( sourceScaling )
          .popInt32( sourceOnScreen )
          .popFloat( sourceSpreading )
          .popInt32( sourceTraitFlags )
          .isOkNoMoreArgs();
        if( !decodeResult )
        {
          std::cerr << "IosonoOscParser: Parsing of OSC packet failed." << std::endl;
          continue;
        }

      }
    }

bool IosonoOscParser::parseObjectMessage( oscpkt::Message const * msg, Packet& packet )
{
  if( msg->match( "/iosono/renderer/version1/src" ) )
  {
    packet.protocol = Protocol::ThreeD;
  }
  else if( msg->match( "/iosono/renderer/verticalpan/v1/src" ) )
  {
    packet.protocol = Protocol::VerticalPan;
  }
  else
  {
    std::cerr << "IosonoOscParser: Received non-matching OSC packet." << std::endl;
    return false;
  }
  int32_t algorithmHint;
  if( !msg->arg()
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
  bool const isPW = ((algorithmHint && (1 << 0)) != 0);
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
    if( packet.sourceSpreading == 1.0f )
    {
      typeId = ObjectTypeId::DiffuseSource;
    }
    else
    {
      typeId = ObjectTypeId::PointSourceWithDiffuseness;
    }
  }
  else if( packet.sourceType == SourceType::PlaneWave )
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
  case ObjectTypeId::PointSource:
  {
    PointSource & psObj = dynamic_cast<PointSource &>(*newObj);
    break;
  }
  default:
    throw std::logic_error( "IosonoOscParser: Unknown source type set.");
  }

  objVec.set( id, *newObj );
}

  
} // namespace convertiosono
} // namespace visr
