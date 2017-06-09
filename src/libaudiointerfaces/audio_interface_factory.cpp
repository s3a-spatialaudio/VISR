/* Copyright Institute of Sound and Vibration Research - All rights reserved */

//#include <librrl/audio_interface.hpp>
#include <libaudiointerfaces/audio_interface_factory.hpp>
#ifdef VISR_JACK_SUPPORT
#include <libaudiointerfaces/jack_interface.hpp>
#endif
#include <libaudiointerfaces/portaudio_interface.hpp>

//#include <libril/parameter_config_base.hpp>

//#include <stdexcept>

namespace visr
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
    
    /*static*/ AudioInterfaceFactory::CreatorTable &
    AudioInterfaceFactory::creatorTable()
    {
        static AudioInterfaceFactory::CreatorTable sCreatorTable;
        return sCreatorTable;
    }
    
    /*static*/ std::unique_ptr<audiointerfaces::AudioInterface>
    AudioInterfaceFactory::create(std::string const & interfaceName, audiointerfaces::AudioInterface::Configuration const & baseConfig, std::string const & config)
    {
        CreatorTable::const_iterator findIt
        = creatorTable().find( interfaceName );
        if( findIt == creatorTable().end() )
        {
            throw std::invalid_argument( "ParameterFactory: No creator function for requested parameter type " );
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
} // namespace visr
