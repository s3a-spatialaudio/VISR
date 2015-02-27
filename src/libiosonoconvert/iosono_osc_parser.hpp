/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_IOSONO_OSC_PARSER_HPP_INCLUDED
#define VISR_OBJECTMODEL_IOSONO_OSC_PARSER_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <string>

#include <oscpkt.hh>

namespace visr
{
namespace objectmodel
{
class ObjectVector;
}
namespace convertiosono
{

/**
 *
 */
class IosonoOscParser
{
public:
  /**
   * Default constructor. Initialises the OSC reader.
   */

  void parse( char const * oscMessageData, std::size_t size,
              objectmodel::ObjectVector & parsedObjects );

  enum class Protocol
  {
    ThreeD, 
    VerticalPan
  };

  struct Packet
  {
  public:
    Protocol protocol;
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

  };

private:
  oscpkt::PacketReader mOscParser;

};

} // namespace convertiosono
} // namespace visr

#endif // VISR_OBJECTMODEL_IOSONO_OSC_PARSER_HPP_INCLUDED
