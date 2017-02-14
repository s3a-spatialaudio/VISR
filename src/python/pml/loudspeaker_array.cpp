/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <boost/noncopyable.hpp>
#include <boost/python.hpp>

#include "loudspeaker_array.hpp"

// This is a temporary hack. Move LoudspeakerArray away from libpanning (and rename!)
#include <libpanning/LoudspeakerArray.h>

using namespace boost::python;

namespace visr
{
using panning::LoudspeakerArray;

namespace python
{
namespace pml
{

void exportLoudspeakerArray()
{
  class_<LoudspeakerArray, boost::noncopyable>( "LoudspeakerArray", boost::python::init<std::string const &>( args("xmlFile") ) )
    .add_property( "numberOfTriplets", &LoudspeakerArray::getNumTriplets )
    .add_property( "numberOfLoudspeakers", &LoudspeakerArray::getNumRegularSpeakers )
    .def( "loadXml", &LoudspeakerArray::loadXml )
    ;
}

} // namepace pml
} // namespace python
} // namespace visr
