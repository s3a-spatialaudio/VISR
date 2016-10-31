/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "options.hpp"
#include "init_filter_matrix.hpp"

#include <libefl/denormalised_number_handling.hpp>

#include <libpml/index_sequence.hpp>

#include <librbbl/fft_wrapper_factory.hpp>

#include <libsignalflows/matrix_convolver.hpp>

#ifdef VISR_JACK_SUPPORT
#include <librrl/jack_interface.hpp>
#endif
#include <librrl/portaudio_interface.hpp>

#include <boost/algorithm/string/predicate.hpp>

#include <cstdlib>
#include <cstdio> // for getc(), for testing purposes
#include <iostream>
#include <memory>
#include <sstream>

int main( int argc, char const * const * argv )
{
  using namespace visr;
  using namespace visr::apps::matrix_convolver;

  efl::DenormalisedNumbers::State const oldDenormNumbersState
    = efl::DenormalisedNumbers::setDenormHandling( );

  Options cmdLineOptions;
  std::stringstream errMsg;
  switch( cmdLineOptions.parse( argc, argv, errMsg ) )
  {
  case Options::ParseResult::Failure:
    std::cerr << "Error while parsing command line options: " << errMsg.str( ) << std::endl;
    return EXIT_FAILURE;
  case Options::ParseResult::Help:
    cmdLineOptions.printDescription( std::cout );
    return EXIT_SUCCESS;
  case Options::ParseResult::Version:
    // TODO: Outsource the version string generation to a central file.
    std::cout << "VISR Matrix convolver utility  "
	      << VISR_MAJOR_VERSION << "."
	      << VISR_MINOR_VERSION << "."
	      << VISR_PATCH_VERSION << std::endl;
    return EXIT_SUCCESS;
  case Options::ParseResult::Success:
    break; // carry on
  }

  // Query options
  if( cmdLineOptions.getDefaultedOption<bool>( "list-audio-backends", false ) )
  {
    std::cout << "Supported audio backends:" << "TBD" << std::endl;
    return EXIT_SUCCESS;
  }
  if( cmdLineOptions.getDefaultedOption<bool>( "list-fft-libraries", false ) )
  {
    std::cout << "Supported FFT libraries:"
              << rbbl::FftWrapperFactory<ril::SampleType>::listImplementations() << std::endl;
    return EXIT_SUCCESS;
  }

  std::string const audioBackend = cmdLineOptions.getDefaultedOption<std::string>( "audio-backend", "default" );

  std::size_t const numberOfInputChannels = cmdLineOptions.getOption<std::size_t>( "input-channels" );
  std::size_t const numberOfOutputChannels = cmdLineOptions.getOption<std::size_t>( "output-channels" );
  std::string const routingsString = cmdLineOptions.getDefaultedOption<std::string>( "routings", "[]" );
  pml::FilterRoutingList const routings( pml::FilterRoutingList::fromJson( routingsString ) );
  const std::size_t maxFilterRoutings = cmdLineOptions.getDefaultedOption<std::size_t>( "max-routings", routings.size() );
  if( routings.size() > maxFilterRoutings )
  {
    throw std::invalid_argument( "The number of initial filter routings exceeds the value specified in \"--maxRoutings\"." );
  }

  // Initialise the impulse response matrix
  // Use max() as special value to denote "no maximum length specified"
  std::size_t const maxFilterLengthOption = cmdLineOptions.getDefaultedOption<std::size_t>( "max-filter-length", std::numeric_limits<std::size_t>::max() );
  std::size_t const maxFilterOption = cmdLineOptions.getDefaultedOption<std::size_t>( "max-filters", std::numeric_limits<std::size_t>::max( ) ); // max() denotes
  std::string const filterList = cmdLineOptions.getDefaultedOption<std::string>( "filters", std::string() );
  std::string const indexOffsetString = cmdLineOptions.getDefaultedOption<std::string>( "filter-file-index-offsets", std::string( ) );
  pml::IndexSequence const indexOffsets( indexOffsetString );
  efl::BasicMatrix<ril::SampleType> initialFilters( ril::cVectorAlignmentSamples );
  initFilterMatrix( filterList, maxFilterLengthOption, maxFilterOption, indexOffsets, initialFilters );

  // The final values for the number and length of filter slots are determined by the logic of the initialisation function.
  std::size_t const maxFilters = initialFilters.numberOfRows();

  std::size_t const periodSize = cmdLineOptions.getDefaultedOption<std::size_t>( "period", 1024 );
  ril::SamplingFrequencyType const samplingFrequency = cmdLineOptions.getDefaultedOption<ril::SamplingFrequencyType>( "sampling-frequency", 48000 );

  std::string const fftLibrary = cmdLineOptions.getDefaultedOption<std::string>( "fft-library", "default" );

  signalflows::MatrixConvolver flow( numberOfInputChannels, numberOfOutputChannels,
                                     initialFilters.numberOfColumns(), maxFilters, maxFilterRoutings,
                                     initialFilters, routings,
                                     fftLibrary.c_str(),
                                     periodSize, samplingFrequency );

  // Selection of audio interface:
  // FOr the moment we check for the name 'JACK' and select the specialized audio interface and fall
  // back to PortAudio in all other cases.
  // TODO: Provide factory and backend-specific options) to make selection of audio interfaces more general and extendable.
#ifdef VISR_JACK_SUPPORT
  bool const useNativeJack = boost::iequals(audioBackend, "JACK" );
#endif

   std::unique_ptr<visr::ril::AudioInterface> audioInterface;

  try 
  {
#ifdef VISR_JACK_SUPPORT
    if( useNativeJack )
    {
      rrl::JackInterface::Config interfaceConfig;
      interfaceConfig.mNumberOfCaptureChannels = numberOfInputChannels;
      interfaceConfig.mNumberOfPlaybackChannels = numberOfOutputChannels;
      interfaceConfig.mPeriodSize = periodSize;
      interfaceConfig.mSampleRate = samplingFrequency;
      interfaceConfig.setCapturePortNames( "input_", 0, numberOfInputChannels - 1 );
      interfaceConfig.setPlaybackPortNames( "output_", 0, numberOfOutputChannels - 1 );
      interfaceConfig.mClientName = "MatrixConvolver";
      audioInterface.reset( new rrl::JackInterface( interfaceConfig ));
    }
    else
#endif
    {
      rrl::PortaudioInterface::Config interfaceConfig;
      interfaceConfig.mNumberOfCaptureChannels = numberOfInputChannels;
      interfaceConfig.mNumberOfPlaybackChannels = numberOfOutputChannels;
      interfaceConfig.mPeriodSize = periodSize;
      interfaceConfig.mSampleRate = samplingFrequency;
      interfaceConfig.mInterleaved = false;
      interfaceConfig.mSampleFormat = rrl::PortaudioInterface::Config::SampleFormat::float32Bit;
      interfaceConfig.mHostApi = audioBackend;
      audioInterface.reset( new rrl::PortaudioInterface( interfaceConfig ) );
    }

    audioInterface->registerCallback( &ril::AudioSignalFlow::processFunction, &flow );

    // should there be a separate start() method for the audio interface?
    audioInterface->start( );

    // Rendering runs until q<Return> is entered on the console.
    std::cout << "S3A matrix convolver running. Press \"q<Return>\" or Ctrl-C to quit." << std::endl;
    char c;
    do
    {
      c = std::getc( stdin );
    }
    while( c != 'q' );

    audioInterface->stop( );

   // Should there be an explicit stop() method for the sound interface?
    audioInterface->unregisterCallback( &ril::AudioSignalFlow::processFunction );

    efl::DenormalisedNumbers::resetDenormHandling( oldDenormNumbersState );
  }
  catch( std::exception const & ex )
  {
    std::cout << "Exception caught on top level: " << ex.what() << std::endl;
    efl::DenormalisedNumbers::resetDenormHandling( oldDenormNumbersState );
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
