/* Copyright Institute of Sound and Vibration Research - All rights reserved */

// Enable native JACK interface instead of PortAudio
// TODO: Make this selectable via a command line option.
// #define NATIVE_JACK

#include <libefl/denormalised_number_handling.hpp>

#ifdef NATIVE_JACK
#include <librrl/jack_interface.hpp>
#else
#include <librrl/portaudio_interface.hpp>
#endif
#include <librrl/audio_signal_flow.hpp>

#include <libsignalflows/time_frequency_feedthrough.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstdio> // for getc(), for testing purposes
#include <iostream>
#include <sstream>

int main( int argc, char const * const * argv )
{
  using namespace visr;

  try
  {
    efl::DenormalisedNumbers::State const oldDenormNumbersState
    = efl::DenormalisedNumbers::setDenormHandling();

    std::size_t const numberOfChannels = 2;
    std::size_t const period = 256;
    std::size_t const windowLength = 2 * period;
    std::size_t const hopSize = period;
    std::size_t const dftSize = 2 * windowLength;

    ril::SamplingFrequencyType const samplingRate = 48000;

#ifdef NATIVE_JACK
    rrl::JackInterface::Config interfaceConfig;
#else
    rrl::PortaudioInterface::Config interfaceConfig;
#endif
    interfaceConfig.mNumberOfCaptureChannels = numberOfChannels;
    interfaceConfig.mNumberOfPlaybackChannels = numberOfChannels;
    interfaceConfig.mPeriodSize = period;
    interfaceConfig.mSampleRate = samplingRate;
#ifdef BASELINE_RENDERER_NATIVE_JACK
    interfaceConfig.setCapturePortNames( "input_", 0, numberOfObjects-1 );
    interfaceConfig.setPlaybackPortNames( "output_", 0, numberOfOutputChannels-1 );
    interfaceConfig.mClientName = "BaselineRenderer";
#else
    interfaceConfig.mInterleaved = false;
    interfaceConfig.mSampleFormat = rrl::PortaudioInterface::Config::SampleFormat::float32Bit;
    interfaceConfig.mHostApi = "default";
#endif

    ril::SignalFlowContext context( period, samplingRate );

    //ril::SignalFlowContext & context,
    //  char const * name,
    //  ril::CompositeComponent * parent,
    //  std::size_t numberOfChannels,
    //  std::size_t dftSize,
    //  std::size_t windowLength,
    //  std::size_t hopSize )

    signalflows::TimeFrequencyFeedthrough flow( context,
                                        "", nullptr,
                                        numberOfChannels,
                                        dftSize,
                                        windowLength,
                                        hopSize );

    rrl::AudioSignalFlow audioFlow( flow );

#ifdef NATIVE_JACK
    rrl::JackInterface audioInterface( interfaceConfig );
#else
    rrl::PortaudioInterface audioInterface( interfaceConfig );
#endif

    audioInterface.registerCallback( &rrl::AudioSignalFlow::processFunction, &audioFlow );

    // should there be a separate start() method for the audio interface?
    audioInterface.start( );

    // Rendering runs until q<Return> is entered on the console.
    std::cout << "S3A time-frequency feedthrough renderer running. Press \"q<Return>\" or Ctrl-C to quit." << std::endl;
    char c;
    do
    {
      c = std::getc( stdin );
    }
    while( c != 'q' );

    audioInterface.stop( );

    audioInterface.unregisterCallback( &rrl::AudioSignalFlow::processFunction );

    efl::DenormalisedNumbers::resetDenormHandling( oldDenormNumbersState );
  }
  catch( std::exception const & ex )
  {
    std::cout << "Exception caught on top level: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
