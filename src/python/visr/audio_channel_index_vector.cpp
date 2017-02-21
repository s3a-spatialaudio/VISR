/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_channel_index_vector.hpp"

#include <libvisr_impl/audio_connection_descriptor.hpp>

#ifdef USE_PYBIND11
#include <pybind11.h>
#include <stl.h> // We should include the base directory instead and say "#include <pybind11/pybind11.h> #include <pybind11/stl.h>"
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

void exportAudioChannelIndexVector( pybind11::module& m )
{


  pybind11::class_<ril::AudioChannelSlice>(m,"AudioChannelSlice")
    .def( pybind11::init<>() )
    .def( pybind11::init<std::size_t, std::size_t, std::ptrdiff_t>(), pybind11::arg("start"), pybind11::arg("end"), pybind11::arg("step")=1 )
//    .def( pybind11::init<pybind11::sequence const &>() )
//    .def( "__init__", [](ril::AudioChannelSlice & instance, pybind11::sequence const &){ new (&instance) ril::AudioChannelSlice(); } )
    .def( "__str__", [](ril::AudioChannelSlice const & slice ){ std::stringstream outStr; outStr << slice.start() << ":"; return outStr.str();} )
    ;


  /**
   * TODO: Decide whether we want additional inspection methods.
   * This would mean that we access the internal() object (probably adding methods to ComponentsWrapper)
   */
  pybind11::class_<ril::AudioChannelIndexVector>(m, "AudioChannelIndexVector" )
    .def( pybind11::init<>() )
    .def( pybind11::init<std::vector<ril::AudioChannelIndexVector::IndexType > const & >() )
    .def( pybind11::init<ril::AudioChannelSlice const & >() )
    .def( pybind11::init<std::list<ril::AudioChannelSlice> const & >() )
    .def( "__str__", [](ril::AudioChannelIndexVector const & self ){ std::stringstream outStr; std::copy(self.begin(), self.end(), std::ostream_iterator<std::size_t>(outStr, ",") ); return outStr.str();} )
    ;
}

#else

#error "AudioChannelIndexVector bindings not supported for boost::python"

#endif

} // namepace visr
} // namespace python
} // namespace visr

