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
#include <libaudiointerfaces/audio_interface.hpp>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace visr
{
    
    // Forward declarations
    class AudioInterface;
    class Configuration;
//    class Config;
//    class ParameterConfigBase;
//    template<class ConcreteType, class ParameterConfig, ParameterType> class TypedParameterBase;
    
    class //VISR_CORE_LIBRARY_SYMBOL 
      AudioInterfaceFactory
    {
    public:
      /**
       ** Creates an istance of the specified audio interface. This is done at runtime, following the factory pattern.
       * @param interfaceName Identifier to specify the audio interface to instantiate
       * @param baseConfig Configuration parameters which are common to all audio interfaces
       * @param config Configuration parameters which are specific for the given audio interface
       */
        static std::unique_ptr<audiointerfaces::AudioInterface> create(std::string const & interfaceName, audiointerfaces::AudioInterface::Configuration const & baseConfig, std::string const & config);
      
      /**
       ** Returns alist of all the instantiable audio interfaces.
       */
      static std::vector<std::string> audioInterfacesList();
      
      /**
       ** Registers a new instantiable audio interface in the factory.
       */
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
            using CreateFunction = std::function< audiointerfaces::AudioInterface* ( audiointerfaces::AudioInterface::Configuration const & baseConfig, std::string const & config  ) >;
            explicit Creator( CreateFunction fcn );

            std::unique_ptr<audiointerfaces::AudioInterface> create( audiointerfaces::AudioInterface::Configuration const & baseConfig, std::string const & config) const;
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
            
            static audiointerfaces::AudioInterface* construct( audiointerfaces::AudioInterface::Configuration const & baseConfig, std::string const & config )
            {
                audiointerfaces::AudioInterface* obj = new ConcreteAudioInterface(baseConfig, config );
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
    
} // namespace visr

#endif /* audio_interface_factory_h */
