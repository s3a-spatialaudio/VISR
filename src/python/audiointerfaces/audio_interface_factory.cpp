/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libaudiointerfaces/audio_interface_factory.hpp> 

#include <libaudiointerfaces/audio_interface.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <vector>

namespace visr
{

using audiointerfaces::AudioInterfaceFactory;

namespace python
{
namespace audiointerfaces
{

void exportAudioInterfaceFactory( pybind11::module & m )
{
  pybind11::class_<AudioInterfaceFactory>( m, "AudioInterfaceFactory" )
    .def_static( "create", &AudioInterfaceFactory::create, pybind11::arg("interfaceName"),
      pybind11::arg("baseConfig"), pybind11::arg("typeSpecificConfig"), pybind11::return_value_policy::take_ownership )
    .def_static( "audioInterfacesList", &AudioInterfaceFactory::audioInterfacesList );
  ;
}

} // namespace rrl
} // namespace python
} // namespace visr
