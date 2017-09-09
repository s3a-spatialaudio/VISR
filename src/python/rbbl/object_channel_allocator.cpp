/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librbbl/object_channel_allocator.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace visr
{
namespace rbbl
{
namespace python
{

namespace py = pybind11;

void exportObjectChannelAllocator( pybind11::module & m )
{
  using visr::rbbl::ObjectChannelAllocator;

  py::class_<ObjectChannelAllocator>( m, "ObjectChannelAllocator")
    .def( py::init< std::size_t >(), py::arg( "numberOfObjectChannels" ) )
    .def( "setObjects", &ObjectChannelAllocator::setObjects, py::arg( "objectIds" ) )
    .def_property_readonly( "maxChannels", &ObjectChannelAllocator::maxChannels )
    .def_property_readonly( "numberOfUsedChannels", &ObjectChannelAllocator::numberUsedChannels )
    .def( "getObjectForChannel", &ObjectChannelAllocator::getObjectForChannel, py::arg( "channelId") )
    .def( "getObjectChannels",
      []( ObjectChannelAllocator const & self )
      {
        std::size_t const numChannels = self.numberUsedChannels();
        std::vector<visr::objectmodel::ObjectId> ret( numChannels );
        for( std::size_t(idx); idx < numChannels; ++idx )
        {
          ret[idx] = self.getObjectForChannel( idx );
        }
        return ret;
      }, "")
  ;
}

} // namespace python
} // namepace rbbl
} // namespace visr
