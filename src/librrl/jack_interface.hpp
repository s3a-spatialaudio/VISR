/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_JACK_INTERFACE_HPP_INCLUDED
#define VISR_LIBRRL_JACK_INTERFACE_HPP_INCLUDED

#include "audio_interface.hpp"

#include <libril/constants.hpp>

#include <memory>
#include <string>
#include <vector>

namespace visr
{
namespace rrl
{

class JackInterface: public rrl::AudioInterface
{
public:
  /**
   * Structure to hold all configuration arguments for a PortAudioInterface instance.
   */
  struct Config
  {
  public:
    /** Default contructor to initialise elements to defined values. */
    Config()
      : mNumberOfCaptureChannels( 0 )
      , mNumberOfPlaybackChannels( 0 )
      , mPeriodSize( 0 )
      , mSampleRate( 0 )
      , mClientName("")
      , mServerName("")
    {}

    void setCapturePortNames( std::string const baseName,
			      std::size_t startIndex,
			      std::size_t endIndex );

    void setPlaybackPortNames( std::string const baseName,
			       std::size_t startIndex,
			       std::size_t endIndex );

    std::size_t mNumberOfCaptureChannels;
    std::size_t mNumberOfPlaybackChannels;

    std::size_t mPeriodSize;

    using SamplingRateType = std::size_t;
    SamplingRateType mSampleRate;

    std::string mClientName;

    std::string mServerName;

    std::vector< std::string > mCapturePortNames;

    std::vector< std::string > mPlaybackPortNames;

  };

  using Base = AudioInterface;

  explicit JackInterface( Config const & config );

  ~JackInterface( );

  /* virtual */ void start();

  /* virtual */ void stop();

  /*virtual*/ bool registerCallback( AudioCallback callback, void* userData );

  /*virtual*/ bool unregisterCallback( AudioCallback audioCallback );
private:
  /**
   * Private implementation class to avoid dependencies to the Portaudio library in the public interface.
   */
  class Impl;

  /**
   * Private implementation object according to the "pointer to implementation" (pimpl) idiom.
   */
  std::unique_ptr<Impl> mImpl;
};

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_JACK_INTERFACE_HPP_INCLUDED
