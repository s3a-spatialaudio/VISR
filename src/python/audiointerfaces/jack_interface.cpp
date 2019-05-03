/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libaudiointerfaces/audio_interface.hpp> 
#include <libaudiointerfaces/jack_interface.hpp>

#include <librrl/audio_signal_flow.hpp> 

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <vector>

namespace visr
{
namespace python
{
namespace audiointerfaces
{


void exportJackInterface( pybind11::module & m )
{

  pybind11::class_<visr::audiointerfaces::JackInterface, visr::audiointerfaces::AudioInterface> ( m, "JackInterface" )
   .def( pybind11::init<visr::audiointerfaces::AudioInterface::Configuration const &, std::string const & >(), pybind11::arg("config"), pybind11::arg("optionalConfig") )
    ;
}

} // namespace audiointerfaces
} // namespace python
} // namespace visr
