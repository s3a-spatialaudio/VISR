/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librrl/audio_interface.hpp>
#include <libaudiointerfaces/audio_interface_factory.hpp>

//#include <libril/parameter_config_base.hpp>

//#include <stdexcept>

namespace visr
{
    
    AudioInterfaceFactory::Creator::Creator( CreateFunction fcn )
    : mCreateFunction( fcn )
    {
    }
    
    std::unique_ptr<rrl::AudioInterface >
    AudioInterfaceFactory::Creator::create( rrl::AudioInterface::Configuration const & baseConfig, std::string const & config ) const
    {
        return std::unique_ptr< rrl::AudioInterface >( mCreateFunction( baseConfig, config ) );
    }
    
    /*static*/ AudioInterfaceFactory::CreatorTable &
    AudioInterfaceFactory::creatorTable()
    {
        static AudioInterfaceFactory::CreatorTable sCreatorTable;
        return sCreatorTable;
    }
    
    /*static*/ std::unique_ptr<rrl::AudioInterface>
    AudioInterfaceFactory::create(std::string const & interfaceName, rrl::AudioInterface::Configuration const & baseConfig, std::string const & config)
    {
        CreatorTable::const_iterator findIt
        = creatorTable().find( interfaceName );
        if( findIt == creatorTable().end() )
        {
            throw std::invalid_argument( "ParameterFactory: No creator function for requested parameter type " );
        }
        // todo: Need to catch construction errors?
        return std::unique_ptr<rrl::AudioInterface>( findIt->second.create( baseConfig, config ) );
    }
    
} // namespace visr
