/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "loudspeaker_array.hpp"

// This is a temporary hack. Move LoudspeakerArray away from libpanning (and rename!)
#include <libpanning/LoudspeakerArray.h>

#include <pybind11/pybind11.h>

namespace visr
{
using panning::LoudspeakerArray;

namespace python
{
namespace pml
{

void exportLoudspeakerArray( pybind11::module & m)
{
  pybind11::class_<LoudspeakerArray>( m, "LoudspeakerArray" )
    .def( pybind11::init<std::string const &>(), pybind11::arg("xmlFile") )
    .def_property_readonly( "numberOfTriplets", &LoudspeakerArray::getNumTriplets )
    .def_property_readonly( "numberOfLoudspeakers", &LoudspeakerArray::getNumRegularSpeakers )
    .def( "loadXml", &LoudspeakerArray::loadXml )
    ;
}

} // namepace pml
} // namespace python
} // namespace visr
