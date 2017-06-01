/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "options.hpp"

#include <libefl/denormalised_number_handling.hpp>
#include <libefl/basic_matrix.hpp>
#include <libefl/vector_functions.hpp>

#include <libpanning/LoudspeakerArray.h>

#include <libpml/initialise_parameter_library.hpp>

#include <libril/signal_flow_context.hpp>

#include <librrl/audio_signal_flow.hpp>
#ifdef VISR_JACK_SUPPORT
#include <libaudiointerfaces/jack_interface.hpp>
#endif
#include <libaudiointerfaces/portaudio_interface.hpp>
#include <librrl/audio_interface.hpp>
#include <libsignalflows/baseline_renderer.hpp>

#include <boost/algorithm/string.hpp> // case-insensitive string compare
#include <boost/filesystem.hpp>
//  #include <boost/string/replace.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstdio> // for getc(), for testing purposes
#include <iostream>
#include <memory>
#include <sstream>

// avoid annoying warning about unsafe standard library functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

int main( int argc, char const * const * argv )
{
  using namespace visr;
  using namespace visr::apps::baseline_renderer;

  try
  {
    // Load all parameters and communication protocols into the respective factories.
    pml::initialiseParameterLibrary();

    efl::DenormalisedNumbers::State const oldDenormNumbersState
      = efl::DenormalisedNumbers::setDenormHandling();

    Options cmdLineOptions;
    std::stringstream errMsg;
    switch (cmdLineOptions.parse(argc, argv, errMsg))
    {
    case Options::ParseResult::Failure:
      std::cerr << "Error while parsing command line options: " << errMsg.str() << std::endl;
      return EXIT_FAILURE;
    case Options::ParseResult::Help:
      cmdLineOptions.printDescription(std::cout);
      return EXIT_SUCCESS;
    case Options::ParseResult::Version:
      // TODO: Outsource the version string generation to a central file.
      std::cout << "VISR S3A Baseline Renderer "
        << VISR_MAJOR_VERSION << "."
        << VISR_MINOR_VERSION << "."
        << VISR_PATCH_VERSION << std::endl;
      return EXIT_SUCCESS;
    case Options::ParseResult::Success:
      break; // carry on
    }

    boost::filesystem::path const arrayConfigPath(cmdLineOptions.getOption<std::string>("array-config"));
    if (!exists(arrayConfigPath))
    {
      std::cerr << "The specified loudspeaker array configuration file \""
        << arrayConfigPath.string() << "\" does not exist." << std::endl;
      return EXIT_FAILURE;
    }
    std::string const arrayConfigFileName = arrayConfigPath.string();
    panning::LoudspeakerArray loudspeakerArray;
    // As long as we have two different config formats, we decide based on the file extention.
    std::string::size_type lastDotIdx = arrayConfigFileName.rfind('.');
    std::string const configfileExtension = lastDotIdx == std::string::npos ? std::string() : arrayConfigFileName.substr(lastDotIdx + 1);
    if (boost::iequals(configfileExtension, std::string("xml")))
    {
      loudspeakerArray.loadXmlFile(arrayConfigFileName);
    }
    else
    {
      FILE* hFile = fopen(arrayConfigFileName.c_str(), "r");
      if (loudspeakerArray.load(hFile) < 0)
      {
        throw std::invalid_argument("Error while parsing the loudspeaker array configuration file \""
          + arrayConfigFileName + "\".");
      }
    }

    const std::size_t numberOfLoudspeakers = loudspeakerArray.getNumRegularSpeakers();
    const std::size_t numberOfSpeakersAndSubs = numberOfLoudspeakers + loudspeakerArray.getNumSubwoofers();

    const std::size_t numberOfOutputChannels
      = cmdLineOptions.getDefaultedOption<std::size_t>("output-channels", numberOfSpeakersAndSubs);

    const std::size_t numberOfObjects = cmdLineOptions.getOption<std::size_t>("input-channels");
    const std::size_t periodSize = cmdLineOptions.getDefaultedOption<std::size_t>("period", 1024);
    const std::size_t numberOfEqSections = cmdLineOptions.getDefaultedOption<std::size_t>("object-eq-sections", 0);
    const std::size_t samplingRate = cmdLineOptions.getDefaultedOption<std::size_t>("sampling-frequency", 48000);

    const std::string audioBackend = cmdLineOptions.getDefaultedOption<std::string>("audio-backend", "default");

    const std::size_t  sceneReceiverPort = cmdLineOptions.getDefaultedOption<std::size_t>("scene-port", 4242);

    const std::string trackingConfiguration = cmdLineOptions.getDefaultedOption<std::string>("tracking", std::string());

    // Initialise with a valid, albeit empty JSON string if no reverb config is provided. This translates to a reverb
    // rendering path with zero reverb object 'slots'
    const std::string reverbConfiguration= cmdLineOptions.getDefaultedOption<std::string>( "reverb-config", "{}" );

    // Selection of audio interface:
    // For the moment we check for the name 'NATIVE_JACK' and select the specialized audio interface and fall
    // back to PortAudio in all other cases.
    // TODO: Provide factory and backend-specific options) to make selection of audio interfaces more general and extendable.
#ifdef VISR_JACK_SUPPORT
    bool const useNativeJack = boost::iequals(audioBackend, "NATIVE_JACK");
#endif

    std::unique_ptr<visr::rrl::AudioInterface> audioInterface;
    visr::rrl::AudioInterface::Configuration const baseConfig(numberOfObjects,numberOfOutputChannels,periodSize,samplingRate);
    const bool lowFrequencyPanning = cmdLineOptions.getDefaultedOption("low-frequency-panning", false );

#ifdef VISR_JACK_SUPPORT
    if (useNativeJack)
    {
      audiointerfaces::JackInterface::Config interfaceConfig;
      interfaceConfig.setCapturePortNames("input_", 0, numberOfObjects - 1);
      interfaceConfig.setPlaybackPortNames("output_", 0, numberOfOutputChannels - 1);
      interfaceConfig.mClientName = "VisrRenderer";
      audioInterface.reset(new audiointerfaces::JackInterface(baseConfig, interfaceConfig));
    }
    else
    {
#endif
      audiointerfaces::PortaudioInterface::Config interfaceConfig;
      interfaceConfig.mInterleaved = false;
      interfaceConfig.mSampleFormat = audiointerfaces::PortaudioInterface::Config::SampleFormat::float32Bit;
      interfaceConfig.mHostApi = audioBackend;
      audioInterface.reset( new audiointerfaces::PortaudioInterface(baseConfig, interfaceConfig) );
#ifdef VISR_JACK_SUPPORT
    }
#endif

    // Assume a fixed length for the interpolation period.
    // Ideally, this roughly matches the update rate of the scene sender.
    const std::size_t cInterpolationLength = std::max( static_cast<std::size_t>(2048), periodSize );

    /* Set up the filter matrix for the diffusion filters. */
    std::size_t const diffusionFilterLength = 63; // fixed filter length of the filters in the compiled-in matrix
    std::size_t const diffusionFiltersInFile = 64; // Fixed number of filters in file.
    // First create a filter matrix containing all filters from a initializer list that is compiled into the program.
    efl::BasicMatrix<SampleType> allDiffusionCoeffs( diffusionFiltersInFile,
                                                          diffusionFilterLength,
#include "files/quasiAllpassFIR_f64_n63_initializer_list.txt"
                                                          , cVectorAlignmentSamples );

    // Create a second filter matrix that matches the number of required filters.
    efl::BasicMatrix<SampleType> diffusionCoeffs( numberOfLoudspeakers, diffusionFilterLength, cVectorAlignmentSamples );
    for( std::size_t idx( 0 ); idx < diffusionCoeffs.numberOfRows( ); ++idx )
    {
      efl::vectorCopy( allDiffusionCoeffs.row( idx ), diffusionCoeffs.row( idx ), diffusionFilterLength, cVectorAlignmentSamples );
    }

    SignalFlowContext context( periodSize, samplingRate );

    signalflows::BaselineRenderer flow( context,
                                        "", nullptr,
                                        loudspeakerArray,
                                        numberOfObjects,
                                        numberOfOutputChannels,
                                        cInterpolationLength,
                                        diffusionCoeffs,
                                        trackingConfiguration,
                                        sceneReceiverPort,
                                        numberOfEqSections,
                                        reverbConfiguration,
                                        lowFrequencyPanning );

    rrl::AudioSignalFlow audioFlow( flow );

    audioInterface->registerCallback( &rrl::AudioSignalFlow::processFunction, &audioFlow );

    // should there be a separate start() method for the audio interface?
    audioInterface->start( );

    // Rendering runs until q<Return> is entered on the console.
    std::cout << "S3A baseline renderer running. Press \"q<Return>\" or Ctrl-C to quit." << std::endl;
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
