/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "options.hpp"

#include <libaudiointerfaces/audio_interface.hpp>
#include <libaudiointerfaces/audio_interface_factory.hpp>

#include <librrl/audio_signal_flow.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <libpythonsupport/python_wrapper.hpp>
#include <libpythonsupport/initialisation_guard.hpp>

#include <libefl/denormalised_number_handling.hpp>

#include <libpml/initialise_parameter_library.hpp>

#include <boost/algorithm/string.hpp> // case-insensitive string compare
#include <boost/filesystem.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstdio> // for getc(), for testing purposes
#include <fstream>
#include <iostream>
#include <sstream>

int main( int argc, char const * const * argv )
{
    using namespace visr;
    using namespace visr::apps::pythonflowrunner;

    try
    {
        efl::DenormalisedNumbers::State const oldDenormNumbersState
        = efl::DenormalisedNumbers::setDenormHandling();

        // Load all parameters and communication protocols into the respective factories.
        pml::initialiseParameterLibrary();

        pythonsupport::InitialisationGuard::initialise();
        // InitialisationGuard::initialise() returns with the Python GIL
        // (global interpreter lock) locked.
        // We keep it locked for the intialisation of the signal flow
        // (which is performed in Python)

        Options cmdLineOptions;
        std::stringstream errMsg;
        switch( cmdLineOptions.parse( argc, argv, errMsg ) )
        {
            case Options::ParseResult::Failure:
                std::cerr << "Error while parsing command line options: " << errMsg.str() << std::endl;
                return EXIT_FAILURE;
            case Options::ParseResult::Help:
                cmdLineOptions.printDescription( std::cout );
                return EXIT_SUCCESS;
            case Options::ParseResult::Version:
                // TODO: Implement retrieval of version information.
                std::cout << "VISR S3A Python signal flow runner V0.9.0" << std::endl;
                return EXIT_SUCCESS;
            case Options::ParseResult::Success:
                break; // carry on
        }
        std::string const audioBackend = cmdLineOptions.getDefaultedOption<std::string>( "audio-backend", "default" );
        const std::size_t periodSize = cmdLineOptions.getDefaultedOption<std::size_t>( "period", 1024 );
        const std::size_t samplingRate = cmdLineOptions.getDefaultedOption<std::size_t>( "sampling-frequency", 48000 );

        boost::filesystem::path const moduleName( cmdLineOptions.getOption<std::string>( "module-name" ) );
        std::string const pythonClassName = cmdLineOptions.getOption<std::string>( "python-class-name");
        std::string const objectName = cmdLineOptions.getDefaultedOption<std::string>( "object-name", "PythonFlow" );
        std::string const positionalArgs = cmdLineOptions.getDefaultedOption<std::string>( "positional-arguments", "" );
        std::string const kwArgs = cmdLineOptions.getDefaultedOption<std::string>( "keyword-arguments", "" );
        boost::filesystem::path const moduleSearchPath = cmdLineOptions.getDefaultedOption<std::string>( "module-search-path", "" );

        if( (not moduleSearchPath.empty() ) and (not exists(moduleSearchPath)) )
        {
          std::cerr << "The specified Python module search path \""
          << moduleSearchPath.string() << "\" does not exist." << std::endl;
          return EXIT_FAILURE;
        }

        SignalFlowContext const ctxt( periodSize, samplingRate );
        pythonsupport::PythonWrapper topLevelComponent( ctxt,
                                                        objectName.c_str(),
                                                        nullptr, // parent component (this is top level)
                                                        moduleName.string().c_str(),
                                                        pythonClassName.c_str(),
                                                        positionalArgs.c_str(),
                                                        kwArgs.c_str(),
                                                        moduleSearchPath.string().c_str() );
        
        rrl::AudioSignalFlow flow( topLevelComponent );
        
        // Note: This works only for single in- and/or output ports of type SampleType.
        std::size_t const numInputs = flow.numberOfCaptureChannels();
        std::size_t const numOutputs = flow.numberOfPlaybackChannels();
        
        // TODO: Check for dangling parameter ports.
        
        visr::audiointerfaces::AudioInterface::Configuration const baseConfig(numInputs,numOutputs,samplingRate,periodSize);
       
        std::string specConf;
        bool const hasAudioInterfaceOptionString = cmdLineOptions.hasOption("audio-ifc-options");
        bool const hasAudioInterfaceOptionFile = cmdLineOptions.hasOption("audio-ifc-option-file");
        if( hasAudioInterfaceOptionString and hasAudioInterfaceOptionFile )
        {
          throw std::invalid_argument( "The options \"--audio-ifc-options\" and \"--audio-ifc-option-file\" cannot both be given.");
        }
        if( hasAudioInterfaceOptionFile )
        {
          boost::filesystem::path const audioIfcConfigFile( cmdLineOptions.getOption<std::string>( "audio-ifc-option-file" ) );
          if( not exists( audioIfcConfigFile ) )
          {
            throw std::invalid_argument( "The file specified by the \"--audio-ifc-option-file\" option does not exist.");
          }
          std::ifstream cfgStream( audioIfcConfigFile.string() );
          if( not cfgStream )
          {
            throw std::invalid_argument( "The file specified by the \"--audio-ifc-option-file\" could not be read.");
          }
          std::ostringstream fileContent;
          fileContent << cfgStream.rdbuf();
          specConf = fileContent.str();
        }
        else
        {
          specConf = hasAudioInterfaceOptionString ? cmdLineOptions.getOption<std::string>( "audio-ifc-options" ) : std::string();
        }

        std::unique_ptr<audiointerfaces::AudioInterface> audioInterface( audiointerfaces::AudioInterfaceFactory::create( audioBackend, baseConfig, specConf) );
        
        audioInterface->registerCallback( &rrl::AudioSignalFlow::processFunction, &flow );

        audioInterface->start();
        
        // Rendering runs until q<Return> is entered on the console.
        std::cout << "S3A Python signal flow runner. Press \"q<Return>\" or Ctrl-C to quit." << std::endl;

        char c;
        do
        {
            c = std::getc( stdin );
        }
        while( c != 'q' );
        
        audioInterface->stop( );
        
        audioInterface->unregisterCallback( &rrl::AudioSignalFlow::processFunction );
        
        efl::DenormalisedNumbers::resetDenormHandling( oldDenormNumbersState );
    }
    catch( std::exception const & ex )
    {
        std::cout << "Exception caught on top level: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
