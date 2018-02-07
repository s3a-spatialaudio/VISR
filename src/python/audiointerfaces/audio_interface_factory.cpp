/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libaudiointerfaces/audio_interface_factory.hpp> 
#include <libaudiointerfaces/audio_interface.hpp>

#include <libvisr/constants.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <vector>

namespace visr
{

using audiointerfaces::AudioInterfaceFactory;
using audiointerfaces::AudioInterface;

namespace python
{
namespace audiointerfaces
{

void exportAudioInterfaceFactory( pybind11::module & m )
{
  pybind11::class_<AudioInterfaceFactory>( m, "AudioInterfaceFactory" )
    .def_static( "create", &AudioInterfaceFactory::create, pybind11::arg("interfaceName"),
      pybind11::arg("baseConfig"), pybind11::arg("optionalConfig") = "", pybind11::return_value_policy::take_ownership )
    .def_static( "create", 
      []( /*AudioInterfaceFactory const & /*self,*/ std::string const & interfaceName, std::size_t numberOfInputs,
          std::size_t numberOfOutputs, std::size_t periodSize, visr::SamplingFrequencyType samplingFrequency, std::string const & optionalConfig )
        {
          AudioInterface::Configuration const conf( numberOfInputs, numberOfOutputs, periodSize, samplingFrequency );
          return AudioInterfaceFactory::create( interfaceName, conf, optionalConfig );
        }, pybind11::arg("interfaceName"), pybind11::arg("numberOfInputs" ), pybind11::arg( "numberOfOutputs"), pybind11::arg( "periodSize" ),
           pybind11::arg( "samplingFrequency"), pybind11::arg( "optionalConfig") = 0 )
    .def_static( "audioInterfacesList", &AudioInterfaceFactory::audioInterfacesList );
  ;
}

} // namespace rrl
} // namespace python
} // namespace visr
