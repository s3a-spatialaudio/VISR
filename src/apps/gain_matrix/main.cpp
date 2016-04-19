/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "options.hpp"

#include <libpml/matrix_parameter.hpp>

#include <librrl/portaudio_interface.hpp>

#include <libsignalflows/gain_matrix.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <cstddef>
#include <cstdlib>
#include <cstdio> // for getc(), for testing purposes
#include <iostream>
#include <memory>

int main( int argc, char const * const * argv )
{
  using namespace visr;
  using namespace visr::apps::gain_matrix;

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
    // TODO: Implement retrieval of version information.
    std::cout << "VISR gain matrix renderer V1.0b" << std::endl;
    return EXIT_SUCCESS;
  case Options::ParseResult::Success:
    break; // carry on
  }

  try 
  {
    std::string const cAudioBackend = cmdLineOptions.getDefaultedOption<std::string>( "audio-backend", "default" );
    const std::size_t periodSize = cmdLineOptions.getDefaultedOption<std::size_t>( "period", 1024 );
    const std::size_t samplingRate = cmdLineOptions.getDefaultedOption<std::size_t>( "sampling-frequency", 48000 );

    if( not( cmdLineOptions.hasOption( "matrix" ) xor cmdLineOptions.hasOption( "matrix-file" ) ) )
    {
      throw std::invalid_argument( "GainMatrix: Exactly one of the options \"--matrix (-m)\" or \"--matrix-file (-f)\" must be provided." );
    }

    std::unique_ptr<pml::MatrixParameter<ril::SampleType> > initialMtx;
    if( cmdLineOptions.hasOption( "matrix" ) )
    {
      initialMtx.reset( new pml::MatrixParameter<ril::SampleType>( pml::MatrixParameter<ril::SampleType>::fromString( cmdLineOptions.getOption<std::string>( "matrix" ),
                             ril::cVectorAlignmentSamples ) ));
    }
    else
    {
      boost::filesystem::path const matrixPath( system_complete( boost::filesystem::path(
        cmdLineOptions.getOption<std::string>( "matrix-file" ) ) ) );
      if( not exists( matrixPath ) or is_directory( matrixPath ) )
      {
        throw std::invalid_argument( std::string("GainMatrix: The file specified by the \"--matrix-file\" argument does not exist." )
          + matrixPath.string() );
      }
      initialMtx.reset( new pml::MatrixParameter<ril::SampleType>( 
        pml::MatrixParameter<ril::SampleType>::fromTextFile( matrixPath.string( ), ril::cVectorAlignmentSamples ) ));
    }
    ril::SampleType const gainAdjustDB = cmdLineOptions.getDefaultedOption<ril::SampleType>( "global-gain", 0.0f );
    // TODO: Replace by library function dB2linear
    ril::SampleType const gainAdjustLinear = std::pow( static_cast<ril::SampleType>(10.0f), gainAdjustDB / static_cast<ril::SampleType>(20.0f) );

    // define fixed parameters for rendering
    const std::size_t numberOfInputs = initialMtx->numberOfColumns();
    const std::size_t numberOfOutputs = initialMtx->numberOfRows();

    for( std::size_t outputIdx( 0 ); outputIdx < numberOfOutputs; ++outputIdx )
    {
      for( std::size_t inputIdx( 0 ); inputIdx < numberOfInputs; ++inputIdx )
      {
        initialMtx->at( outputIdx, inputIdx ) *= gainAdjustLinear;
      }
    }

    rrl::PortaudioInterface::Config interfaceConfig;
    interfaceConfig.mNumberOfCaptureChannels = numberOfInputs;
    interfaceConfig.mNumberOfPlaybackChannels = numberOfOutputs;
    interfaceConfig.mPeriodSize = periodSize;
    interfaceConfig.mSampleRate = samplingRate;
    interfaceConfig.mInterleaved = false;
    interfaceConfig.mSampleFormat = rrl::PortaudioInterface::Config::SampleFormat::float32Bit;
    interfaceConfig.mHostApi = cAudioBackend;

    rrl::PortaudioInterface audioInterface( interfaceConfig );

    // Unused at the moment (no gain changes).
    const std::size_t cInterpolationLength = periodSize;

    visr::signalflows::GainMatrix flow( numberOfInputs, numberOfOutputs,
                                        *initialMtx,
                                        cInterpolationLength, periodSize,
                                        samplingRate );

    audioInterface.registerCallback( &ril::AudioSignalFlow::processFunction, &flow );

    // should there be a separate start() method for the audio interface?
    audioInterface.start( );

    // Rendering runs until <Return> is entered on the console.
    std::getc( stdin );

    audioInterface.stop( );

   // Should there be an explicit stop() method for the sound interface?

    audioInterface.unregisterCallback( &ril::AudioSignalFlow::processFunction );
  }
  catch( std::exception const & ex )
  {
    std::cout << "Exception caught on top level: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
