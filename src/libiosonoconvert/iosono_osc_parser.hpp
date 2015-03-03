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
   * The "panning elevation" feature is set to a standard value (30 degree elevation).
   * This panning elevation is used in one of the supported message formats ("multilayer 3D"),
   * where a relative linear panning ratio between the main horizontal loudspeaker ring and an additional height layer is 
   * given instead of an explicit elevation angle. In the VISR baseline renderer, we use the "panning elevation" angle to reconstruct an object 
   * elevation from the panning ratio.
   */
  IosonoOscParser();

  /**
   * Constructor with an explicit argument for the assumed elevation angle in the 'multilayer 3D' mode.
   * @param panningElevationDegree Elevation angle of the elevated speaker ring, which is used to reconstruct the object coordiantes from
   * the elevation panning ratio.
   */
  explicit IosonoOscParser( float panningElevationDegree );
  
  ~IosonoOscParser();
  

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
   * The elevation angle of an assumed elevated ring of elevated loudspeakers.
   * Value in radian.
   * @see constructors
   */
  float cPanningElevation;

  /**
   * Reads the next packet stored in the parser and stores it in the \p packet parameter.
   * @param msg A message object as returned by the o
   * @param[out] packet The data structure to store the parse object into.
   */
  bool parseObjectMessage( oscpkt::Message const & msg, Packet& packet );


  void writeToObjectVector( Packet const & packet, objectmodel::ObjectVector & objVec );
};

} // namespace convertiosono
} // namespace visr

#endif // VISR_OBJECTMODEL_IOSONO_OSC_PARSER_HPP_INCLUDED
