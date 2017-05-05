/* Copyright Institute of Sound and Vibration Research - All rights reserved */

// Enable native JACK interface instead of PortAudio
// TODO: Make this selectable via a command line option.
// #define BASELINE_RENDERER_NATIVE_JACK

#include "options.hpp"

#include <libril/signal_flow_context.hpp>

#ifdef BASELINE_RENDERER_NATIVE_JACK
#include <librrl/jack_interface.hpp>
#else
#include <librrl/portaudio_interface.hpp>
#endif
#include <librrl/audio_signal_flow.hpp>

#include <libpythonsupport/python_wrapper.hpp>
#include <libpythonsupport/initialisation_guard.hpp>

#include <libefl/denormalised_number_handling.hpp>

#include <boost/algorithm/string.hpp> // case-insensitive string compare
#include <boost/filesystem.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstdio> // for getc(), for testing purposes
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

    pythonsupport::InitialisationGuard::initialise();

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
    //registerOption<std::string>( "audio-backend,D", "The audio backend." );
    //registerOption<std::size_t>( "sampling-frequency,f", "Sampling frequency [Hz]" );
    //registerOption<std::size_t>( "period,p", "Period (blocklength) [Number of samples per audio block]" );

    //registerOption<std::string>( "module-path,m", "Full pathe of the Python module to be loaded." );
    //registerOption<std::size_t>( "python-class-name,c", "Name of the Python class (must be a subclass of visr.Component)." );
    //registerOption<std::size_t>( "positional-arguments,p", "Comma-separated list of positional options passed to the class constructor." );
    //registerOption<std::size_t>( "keyword-arguments,k", "Comma-separated list of named (keyword) options passed to the class constructor." );

    std::string const audioBackend = cmdLineOptions.getDefaultedOption<std::string>( "audio-backend", "default" );
    const std::size_t periodSize = cmdLineOptions.getDefaultedOption<std::size_t>( "period", 1024 );
    const std::size_t samplingRate = cmdLineOptions.getDefaultedOption<std::size_t>( "sampling-frequency", 48000 );

    boost::filesystem::path const modulePath( cmdLineOptions.getOption<std::string>( "module-path" ) );
    if( not exists(modulePath) )
    {
      std::cerr << "The specified Python module path \""
        << modulePath.string() << "\" does not exist." << std::endl;
      return EXIT_FAILURE;
    }
    std::string const pythonClassName = cmdLineOptions.getOption<std::string>( "python-class-name");
    std::string const objectName = cmdLineOptions.getDefaultedOption<std::string>( "python-class-name", "PythonFlow" );
    std::string const positionalArgs = cmdLineOptions.getOption<std::string>( "positional-arguments" );
    std::string const kwArgs = cmdLineOptions.getOption<std::string>( "keyword-arguments" );

    SignalFlowContext const ctxt( periodSize, samplingRate );
    pythonsupport::PythonWrapper topLevelComponent( ctxt, objectName.c_str(), nullptr, 
                                                    modulePath.string().c_str(), pythonClassName.c_str(), 
                                                    positionalArgs.c_str(), kwArgs.c_str() );

    rrl::AudioSignalFlow flow( topLevelComponent );

    // Note: This works only for single in- and/or output ports of type SampleType.
    std::size_t const numInputs = flow.numberOfCaptureChannels();
    std::size_t const numOutputs = flow.numberOfPlaybackChannels();

    // TODO: Check for dangling parameter ports.

#ifdef VISR_JACK_SUPPORT
    bool const useNativeJack = boost::iequals( audioBackend, "NATIVE_JACK" );
#endif

    std::unique_ptr<visr::rrl::AudioInterface> audioInterface;

#ifdef VISR_JACK_SUPPORT
    if( useNativeJack )
    {
      rrl::JackInterface::Config interfaceConfig;
      interfaceConfig.mNumberOfCaptureChannels = numInputs;
      interfaceConfig.mNumberOfPlaybackChannels = numOutputs;
      interfaceConfig.mPeriodSize = periodSize;
      interfaceConfig.mSampleRate = samplingRate;
      interfaceConfig.setCapturePortNames( "input_", 0, numberOfObjects - 1 );
      interfaceConfig.setPlaybackPortNames( "output_", 0, numberOfOutputChannels - 1 );
      interfaceConfig.mClientName = "VisrRenderer";
      audioInterface.reset( new rrl::JackInterface( interfaceConfig ) );
    }
    else
    {
#endif
      rrl::PortaudioInterface::Config interfaceConfig;
      interfaceConfig.mNumberOfCaptureChannels = numInputs;
      interfaceConfig.mNumberOfPlaybackChannels = numOutputs;
      interfaceConfig.mPeriodSize = periodSize;
      interfaceConfig.mSampleRate = samplingRate;
      interfaceConfig.mInterleaved = false;
      interfaceConfig.mSampleFormat = rrl::PortaudioInterface::Config::SampleFormat::float32Bit;
      interfaceConfig.mHostApi = audioBackend;
      audioInterface.reset( new rrl::PortaudioInterface( interfaceConfig ) );
#ifdef VISR_JACK_SUPPORT
    }
#endif

    audioInterface->registerCallback( &rrl::AudioSignalFlow::processFunction, &flow );

    // should there be a separate start() method for the audio interface?
    audioInterface->start( );

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
