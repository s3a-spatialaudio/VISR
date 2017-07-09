/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpanning/LoudspeakerArray.h>
#include <libpml/biquad_parameter.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <vector>

namespace visr
{
using panning::LoudspeakerArray;

namespace python
{
namespace panning
{

namespace
{

/** Internal function to create the array of channel indices */
std::vector<LoudspeakerArray::ChannelIndex> getChannelIndices( LoudspeakerArray const & array)
{
  std::size_t numCh = array.getNumRegularSpeakers();
  if( numCh == 0 )
  {
    return std::vector<LoudspeakerArray::ChannelIndex>();
  }
  else
  {
    return std::vector<LoudspeakerArray::ChannelIndex>(
      array.getLoudspeakerChannels(), array.getLoudspeakerChannels() + numCh );
  }
}

/** 
 * Local function to transform the subwoofer index list into std::vector
 */
std::vector<LoudspeakerArray::ChannelIndex> getSubwooferChannelIndices( LoudspeakerArray const & array )
{

  std::size_t const numSubs = array.getNumSubwoofers();
  if( numSubs == 0 )
  {
    return std::vector<LoudspeakerArray::ChannelIndex>();
  }
  else
  {
    return std::vector<LoudspeakerArray::ChannelIndex>(
      array.getSubwooferChannels(), array.getSubwooferChannels() + numSubs );
  }
}

}

void exportLoudspeakerArray( pybind11::module & m)
{
  pybind11::class_<LoudspeakerArray>(m, "LoudspeakerArray")
    .def(pybind11::init<std::string const &>(), pybind11::arg("xmlFile"))
    .def("loadXmlFile", &LoudspeakerArray::loadXmlFile)
    .def( "loadXmlString", &LoudspeakerArray::loadXmlString)
    .def("getPosition", (visr::panning::XYZ &(LoudspeakerArray::*)(size_t iSpk)) &LoudspeakerArray::getPosition)
    .def("getPosition", (visr::panning::XYZ const &(LoudspeakerArray::*)(size_t iSpk) const) &LoudspeakerArray::getPosition)
    .def( "getPosition", (visr::panning::XYZ &(LoudspeakerArray::*)(LoudspeakerArray::LoudspeakerIdType const & iSpk)) &LoudspeakerArray::getPosition )
    .def( "getPosition", (visr::panning::XYZ const &(LoudspeakerArray::*)(LoudspeakerArray::LoudspeakerIdType const &) const)(&LoudspeakerArray::getPosition) )
    .def( "getPositions", (visr::panning::XYZ *(LoudspeakerArray::*)()) &LoudspeakerArray::getPositions)
    .def( "getPositions", (visr::panning::XYZ const *(LoudspeakerArray::*)() const) &LoudspeakerArray::getPositions)
    .def( "getSpeakerChannelIndex", &LoudspeakerArray::channelIndex, pybind11::arg("speakerIndex"))
    .def( "getSpeakerIndexFromId", &LoudspeakerArray::getSpeakerIndexFromId, pybind11::arg( "speakerId" ) )
    .def( "getSpeakerChannel", &LoudspeakerArray::getSpeakerChannel, pybind11::arg( "speakerIndex" ) )
    .def( "getSpeakerChannelFromId", &LoudspeakerArray::getSpeakerChannelFromId, pybind11::arg( "speakerId" ) )
    .def( "setTriplet", &LoudspeakerArray::setTriplet, pybind11::arg("tripletIndex"), pybind11::arg("speakerId1"), pybind11::arg("speakerId2"), pybind11::arg("speakerId3"))
    .def( "getTriplet", (LoudspeakerArray::TripletType &(LoudspeakerArray::*)(size_t iTri))&LoudspeakerArray::getTriplet)
    .def( "getTriplet", (LoudspeakerArray::TripletType const &(LoudspeakerArray::*)(size_t iTri) const) &LoudspeakerArray::getTriplet)
    .def_property_readonly("totNumberOfLoudspeakers", &LoudspeakerArray::getNumSpeakers)
    .def_property_readonly("numberOfRegularLoudspeakers", &LoudspeakerArray::getNumRegularSpeakers)
    .def_property_readonly("numberOfTriplets", &LoudspeakerArray::getNumTriplets)
    .def_property_readonly("is2D", &LoudspeakerArray::is2D)
    .def_property_readonly("isInfinite", &LoudspeakerArray::isInfinite)
    .def_property_readonly("numberOfSubwoofers", &LoudspeakerArray::getNumSubwoofers)
    .def("subwooferChannelIndices", &getSubwooferChannelIndices )
    .def("subwooferChannelIndex", &LoudspeakerArray::getSubwooferChannel, pybind11::arg("subwooferIndex"))
    .def("getSubwooferGains", &LoudspeakerArray::getSubwooferGains, pybind11::return_value_policy::reference)
    .def("getReRoutingCoefficients", &LoudspeakerArray::getReroutingCoefficients, pybind11::return_value_policy::reference)
    .def("getReRoutingCoefficient", &LoudspeakerArray::getReroutingCoefficient, pybind11::arg("virtualSpeakerIdx"), pybind11::arg("realSpeakerIdx"))
    .def("getLoudspeakerGainAdjustment", &LoudspeakerArray::getLoudspeakerGainAdjustment, pybind11::arg("speakerIdx"))
    .def("getLoudspeakerDelayAdjustment", &LoudspeakerArray::getLoudspeakerDelayAdjustment, pybind11::arg("speakerIdx"))
    .def("getSubwooferGainAdjustment", &LoudspeakerArray::getSubwooferGainAdjustment, pybind11::arg("speakerIdx"))
    .def("getSubwooferDelayAdjustment", &LoudspeakerArray::getSubwooferDelayAdjustment, pybind11::arg("speakerIdx"))
    .def("getGainAdjustments", &LoudspeakerArray::getGainAdjustment)
    .def("getDelayAdjustments", &LoudspeakerArray::getDelayAdjustment)
    .def("isOutputEqualisationPresent", &LoudspeakerArray::outputEqualisationPresent)
    .def("outputEqualisationNumberOfBiquads", &LoudspeakerArray::outputEqualisationNumberOfBiquads)
    .def("outputEqualisationBiquads", &LoudspeakerArray::outputEqualisationBiquads)
    .def( "channelIndices", &getChannelIndices )
    ;
}
} // namepace panning
} // namespace python
} // namespace visr
