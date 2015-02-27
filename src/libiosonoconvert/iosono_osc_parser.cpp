/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "iosono_osc_parser.hpp"

#include <libobjectmodel/object_vector.hpp>

#include <oscpkt.hh>

#include <cstdint>
#include <iostream>

namespace visr
{
namespace convertiosono
{

void IosonoOscParser::parse( char const * oscMessageData,
                             std::size_t size,
                             objectmodel::ObjectVector & parsedObjects )
{
  mOscParser.init( oscMessageData, size );

  oscpkt::Message* msg;
  while( (msg = mOscParser.popMessage()) != nullptr )
  {
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
  }
}
  
} // namespace convertiosono
} // namespace visr
