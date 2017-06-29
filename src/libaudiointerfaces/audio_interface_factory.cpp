/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_interface_factory.hpp"

#include "audio_interface.hpp"

#ifdef VISR_JACK_SUPPORT
#include <libaudiointerfaces/jack_interface.hpp>
#endif
#include <libaudiointerfaces/portaudio_interface.hpp>

namespace visr
{
namespace audiointerfaces
{
    AudioInterfaceFactory::Creator::Creator( CreateFunction fcn )
    : mCreateFunction( fcn )
    {
    }
    std::unique_ptr<audiointerfaces::AudioInterface >
    AudioInterfaceFactory::Creator::create( audiointerfaces::AudioInterface::Configuration const & baseConfig, std::string const & config ) const
    {
        return std::unique_ptr< audiointerfaces::AudioInterface >( mCreateFunction( baseConfig, config ) );
    }
  /**
   ** Returns alist of all the instantiable audio interfaces.
   */
   std::vector<std::string> AudioInterfaceFactory::audioInterfacesList(){
     std::vector <std::string> audioifcs;
     for( CreatorTable::const_iterator it = creatorTable().begin(); it != creatorTable().end(); ++it ) {
       audioifcs.push_back( it->first );
     }
     return audioifcs;
  }
  
    /*static*/ AudioInterfaceFactory::CreatorTable &
    AudioInterfaceFactory::creatorTable()
    {
        static AudioInterfaceFactory::CreatorTable sCreatorTable;
        return sCreatorTable;
    }

  /**
   * Creates an instance of the specified audio interface. This is done at runtime, following the factory pattern.
   * @param interfaceName Identifier to specify the audio interface to instantiate
   * @param baseConfig Configuration parameters which are common to all audio interfaces
   * @param config Configuration parameters which are specific for the given audio interface
   */
    /*static*/ std::unique_ptr<audiointerfaces::AudioInterface>
    AudioInterfaceFactory::create(std::string const & interfaceName, audiointerfaces::AudioInterface::Configuration const & baseConfig, std::string const & config)
    {
        CreatorTable::const_iterator findIt
        = creatorTable().find( interfaceName );
        if( findIt == creatorTable().end() )
        {
            throw std::invalid_argument( std::string("AudioInterfaceFactory: Audio interface with name \"") + interfaceName + "\" is not registered." );
        }
        // todo: Need to catch construction errors?
        return std::unique_ptr<audiointerfaces::AudioInterface>( findIt->second.create( baseConfig, config ) );
    }
    
    
    
    /**
     * A helper class with whole purpose is to register the different object types in the factory.
     */
    struct InstantiateAudioInterfaceFactory
    {
        InstantiateAudioInterfaceFactory()
        {
#ifdef VISR_JACK_SUPPORT
            AudioInterfaceFactory::registerAudioInterfaceType<audiointerfaces::JackInterface>("Jack" );
#endif
            AudioInterfaceFactory::registerAudioInterfaceType<audiointerfaces::PortaudioInterface>("PortAudio" );
           
        }
    };
    
    /**
     * Object which is used to initialise the object factory.
     */
    InstantiateAudioInterfaceFactory const cInstantiationHelper;

} // namespace audiointerfaces
} // namespace visr
