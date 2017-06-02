//
//  audio_interface_factory.h
//  VISR
//
//  Created by Costantini G. on 31/05/2017.
//
//

#ifndef audio_interface_factory_h
#define audio_interface_factory_h




/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/export_symbols.hpp>
//#include "parameter_type.hpp"
//#include <librrl/audio_interface.hpp>

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace visr
{
    
    // Forward declarations
    class AudioInterface;
    class Configuration;
//    class Config;
//    class ParameterConfigBase;
//    template<class ConcreteType, class ParameterConfig, ParameterType> class TypedParameterBase;
    
    class VISR_CORE_LIBRARY_SYMBOL AudioInterfaceFactory
    {
    public:
        static std::unique_ptr<rrl::AudioInterface> create(std::string const & interfaceName, rrl::AudioInterface::Configuration const & baseConfig, std::string const & config);
        
        template< class ConcreteAudioInterface >
        static void registerAudioInterfaceType( std::string const & interfaceName );
        
        
        /**
         * Template class to register parameter types.
         * Creating a static object invokes the class registration function.
         */
        template< class ConcreteAudioInterface >
        class Registrar
        {
        public:
            explicit Registrar(std::string const & interfaceName )
            {
                creatorTable().insert( std::make_pair( interfaceName, TCreator<ConcreteAudioInterface>() ) );
            }
        };
        
    private:
        struct Creator
        {
            using CreateFunction = std::function< rrl::AudioInterface* ( rrl::AudioInterface::Configuration const & baseConfig, std::string const & config  ) >;
            
            VISR_CORE_LIBRARY_SYMBOL explicit Creator( CreateFunction fcn );
            
            VISR_CORE_LIBRARY_SYMBOL std::unique_ptr<rrl::AudioInterface> create( rrl::AudioInterface::Configuration const & baseConfig, std::string const & config) const;
        private:
            CreateFunction mCreateFunction;
        };
        
        template< class ConcreteAudioInterface >
        class TCreator: public Creator
        {
        public:
            TCreator( )
            : Creator( &TCreator<ConcreteAudioInterface>::construct )
            {
            }
            
            static rrl::AudioInterface* construct( rrl::AudioInterface::Configuration const & baseConfig, std::string const & config )
            {
                rrl::AudioInterface* obj = new ConcreteAudioInterface(baseConfig, config );
                return obj;
            }
        };
        
        using CreatorTable = std::map<std::string, Creator >;
        
        static CreatorTable & creatorTable();
    };
    
    template< class ConcreteAudioInterface >
    void AudioInterfaceFactory::registerAudioInterfaceType( std::string const & interfaceName )
    {
        creatorTable().insert( std::make_pair( interfaceName, TCreator<ConcreteAudioInterface>() ) );
    }
//    
//    template< class TypedParameterType >
//    void AudioInterfaceFactory::registerParameterType()
//    {
//        registerParameterType<TypedParameterType>( TypedParameterType::staticType() );
//    }
//    
    
    // The macro does not work for multiple uses in the same .cpp file
    // (multiple definitions of 'maker'), stringization of names difficult
    // because of template brackets and namespace names.
    // #define REGISTER_PARAMETER( type, id ) namespace { static ParameterFactory::Registrar< type > maker( id ); }
    
} // namespace visr

#endif /* audio_interface_factory_h */
