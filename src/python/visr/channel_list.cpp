/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "channel_list.hpp"

#include <libril/channel_list.hpp>

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#endif

#include <algorithm>
#include <ciso646>
#include <iterator>
#include <sstream>

namespace visr
{
namespace python
{
namespace visr
{

#ifdef USE_PYBIND11

void exportChannelList( pybind11::module& m )
{
  pybind11::class_<ChannelRange>(m,"ChannelRange")
    .def( pybind11::init<>() )
    .def( pybind11::init<std::size_t, std::size_t, std::ptrdiff_t>(), pybind11::arg("start"), pybind11::arg("end"), pybind11::arg("step")=1 )
    .def( "__str__", []( ChannelRange const & slice ){ std::stringstream outStr; outStr << slice.start() << ":" << slice.step() << ":" << slice.end(); return outStr.str();} )
    ;

  /**
   * TODO: Decide whether we want additional inspection methods.
   * This would mean that we access the internal() object (probably adding methods to ComponentsWrapper)
   */
  pybind11::class_<ChannelList>(m, "ChannelList" )
    .def( pybind11::init<>() )
    .def( pybind11::init<std::initializer_list<ChannelList::IndexType > const & >() )
    .def( pybind11::init<std::list<ChannelList::IndexType > const & >() )
    .def( pybind11::init<ChannelRange const & >() )
//    .def( pybind11::init<std::list<CompositeComponent::ChannelRange> const & >() )
//    .def( "__init__", []( ChannelList & inst, pybind11:: sequence const & s ) { new (&inst) ChannelList; } )
    .def( "__str__", [](ChannelList const & self ){ std::stringstream outStr; std::copy(self.begin(), self.end(), std::ostream_iterator<std::size_t>(outStr, ",") ); return outStr.str();} )
    ;
}

#else

#error "ChannelList bindings not supported for boost::python"

#endif

} // namepace visr
} // namespace python
} // namespace visr

