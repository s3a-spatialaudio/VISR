/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "options.hpp"

#include <libpml/matrix_parameter.hpp>

#include <libril/signal_flow_context.hpp>

#include <libaudiointerfaces/audio_interface_factory.hpp>
#include <libaudiointerfaces/portaudio_interface.hpp>
#include <libaudiointerfaces/audio_interface.hpp>
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

    std::unique_ptr<pml::MatrixParameter<SampleType> > initialMtx;
    if( cmdLineOptions.hasOption( "matrix" ) )
    {
      initialMtx.reset( new pml::MatrixParameter<SampleType>( pml::MatrixParameter<SampleType>::fromString( cmdLineOptions.getOption<std::string>( "matrix" ),
                             cVectorAlignmentSamples ) ));
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
      initialMtx.reset( new pml::MatrixParameter<SampleType>( 
        pml::MatrixParameter<SampleType>::fromTextFile( matrixPath.string( ), cVectorAlignmentSamples ) ));
    }
    SampleType const gainAdjustDB = cmdLineOptions.getDefaultedOption<SampleType>( "global-gain", 0.0f );
    // TODO: Replace by library function dB2linear
    SampleType const gainAdjustLinear = std::pow( static_cast<SampleType>(10.0f), gainAdjustDB / static_cast<SampleType>(20.0f) );

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
      
    visr::audiointerfaces::AudioInterface::Configuration const baseConfig(numberOfInputs,numberOfOutputs,samplingRate,periodSize);
      std::string type;
      std::string specConf;
      
      specConf = "{\"sampleformat\": 8, \"interleaved\": \"false\", \"hostapi\" : "+cAudioBackend+"}";
      type = "PortAudio";
      
      std::unique_ptr<audiointerfaces::AudioInterface> audioInterface = audiointerfaces::AudioInterfaceFactory::create( type, baseConfig, specConf);

    // Unused at the moment (no gain changes).
    const std::size_t cInterpolationLength = periodSize;

    SignalFlowContext context( periodSize, samplingRate );

    visr::signalflows::GainMatrix flow( context, "", nullptr,
                                        numberOfInputs, numberOfOutputs,
                                        *initialMtx,
                                        cInterpolationLength );

    // audioInterface.registerCallback( &AudioSignalFlow::processFunction, &flow );

    // should there be a separate start() method for the audio interface?
    audioInterface->start( );

    // Rendering runs until <Return> is entered on the console.
    std::getc( stdin );

    audioInterface->stop( );

    // audioInterface.unregisterCallback( &AudioSignalFlow::processFunction );
  }
  catch( std::exception const & ex )
  {
    std::cout << "Exception caught on top level: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
