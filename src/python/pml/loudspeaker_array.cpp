/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "loudspeaker_array.hpp"

// This is a temporary hack. Move LoudspeakerArray away from libpanning (and rename!)
#include <libpanning/LoudspeakerArray.h>

#ifdef USE_PYBIND11
#include <pybind11.h>
#else
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#endif



namespace visr
{
using panning::LoudspeakerArray;

namespace python
{
namespace pml
{

#ifdef USE_PYBIND11
void exportLoudspeakerArray( pybind11::module & m)
{
  pybind11::class_<LoudspeakerArray>( m, "LoudspeakerArray" )
    .def( pybind11::init<std::string const &>(), pybind11::arg("xmlFile") )
    .def_property_readonly( "numberOfTriplets", &LoudspeakerArray::getNumTriplets )
    .def_property_readonly( "numberOfLoudspeakers", &LoudspeakerArray::getNumRegularSpeakers )
    .def( "loadXml", &LoudspeakerArray::loadXml )
    ;
  }
#else
using namespace boost::python;

void exportLoudspeakerArray()
{
  class_<LoudspeakerArray, boost::noncopyable>( "LoudspeakerArray", boost::python::init<std::string const &>( args("xmlFile") ) )
    .add_property( "numberOfTriplets", &LoudspeakerArray::getNumTriplets )
    .add_property( "numberOfLoudspeakers", &LoudspeakerArray::getNumRegularSpeakers )
    .def( "loadXml", &LoudspeakerArray::loadXml )
    ;
}
#endif
} // namepace pml
} // namespace python
} // namespace visr
