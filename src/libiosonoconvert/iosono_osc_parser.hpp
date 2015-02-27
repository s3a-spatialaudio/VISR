/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_IOSONO_OSC_PARSER_HPP_INCLUDED
#define VISR_OBJECTMODEL_IOSONO_OSC_PARSER_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

// _CRT_SECURE_NO_WARNINGS
//#include <oscpkt.hh>

namespace oscpkt
{
class PacketReader;
class Message;
}

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
  IosonoOscParser();

  void parse( char const * oscMessageData, std::size_t size,
              objectmodel::ObjectVector & parsedObjects );

  enum class Protocol
  {
    ThreeD, 
    VerticalPan
  };

  enum class SourceType
  {
    PointSource,
    PlaneWave
  };

  struct Packet
  {
  public:
    Protocol protocol;
    int32_t channelNumber;
    SourceType sourceType;
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
  std::unique_ptr<oscpkt::PacketReader> mOscParser;

  /**
   * Reads the next packet stored in the parser and stores it in the \p packet parameter.
   * @param[out] packet The data structure to store the parse object into.
   */
  bool parseObjectMessage( oscpkt::Message const * msg, Packet& packet );

  void writeToObjectVector( Packet const & packet, objectmodel::ObjectVector & objVec );
};

} // namespace convertiosono
} // namespace visr

#endif // VISR_OBJECTMODEL_IOSONO_OSC_PARSER_HPP_INCLUDED
