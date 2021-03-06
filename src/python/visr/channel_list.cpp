/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libvisr/channel_list.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>

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
    .def( "__str__", [](ChannelList const & self ){ std::stringstream outStr; outStr << self; return outStr.str();} )
    .def_property_readonly( "size", &ChannelList::size)
    .def_property_readonly( "__len__", &ChannelList::size )
    .def( "__getitem__", static_cast<ChannelList::IndexType(ChannelList::*)(std::size_t)const>(&ChannelList::at) )
    .def( "__setitem__", [](ChannelList& cl, std::size_t index, ChannelList::IndexType val ){ cl.at(index) =val; } )
    .def("__iter__", [](ChannelList &v) { return pybind11::make_iterator(v.begin(), v.end());} )
    ;
}

} // namepace visr
} // namespace python
} // namespace visr

