/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_AUDIO_INTERFACE_HPP_INCLUDED
#define VISR_LIBRRL_AUDIO_INTERFACE_HPP_INCLUDED

#include <cstddef>
#include <string>

namespace visr
{
namespace audiointerfaces
{

/**
 * Abstract interface for various audio interfaces.
 */
class AudioInterface
{
public:

  /**
   * The type used to specify the sampling frequency
   * @note Maybe this type should be moved to a more general location & a uniform type is used for the sample rate.
   */
  using SampleRateType = std::size_t;

  class Configuration
  {
  public:
    explicit Configuration( std::size_t numCaptureChannels,
                            std::size_t numPlaybackChannels,
                            SampleRateType sampleRate = 0,
                            std::size_t periodSize = 0
                            );

    virtual ~Configuration();

    /**
     * Access methods to the data members
     */
    //@{
    std::size_t numCaptureChannels() const { return mNumCaptureChannels; }
    std::size_t numPlaybackChannels( ) const { return mNumPlaybackChannels; }
    SampleRateType sampleRate() const { return mSampleRate; }
    std::size_t periodSize( ) const { return mPeriodSize; }

    //@}
  private:
    std::size_t const mNumCaptureChannels;
    std::size_t const mNumPlaybackChannels;
    SampleRateType const mSampleRate;
    std::size_t const mPeriodSize;
  };

  /// Preliminary definition of sample types
  //@{
  using ExternalSampleType = float;

  /// note: the internal sample type should be of no concern to the abstract interface.
  using InternalSampleType = float;
  //@}

  /**
   * The type of the callback function that can be registered to be called if sufficient data is available 
  */
  //typedef void ( *AudioCallback )( void* /* userData */,
  //                               ExternalSampleType const * const * /*captureBuffer*/,
  //                               ExternalSampleType * const * /*playbackBuffer*/,
  //                               bool& /*errorCode*/);

  using AudioCallback = void (*)( void *, ExternalSampleType const * const * /*captureBuffer*/,
                                  ExternalSampleType * const * /*playbackBuffer*/,
                                  bool& /*errorCode*/ );

  virtual bool registerCallback( AudioCallback callbackPtr, void* userData ) = 0;

  virtual bool unregisterCallback( AudioCallback callbackPtr ) = 0;

  /**
   * Start the audio interface, i.e., reacting to callbacks and passing them to the registered callback
   * handlers.
   * Pure virtual method, must be implemented in all concrete derived classes.
   */
  virtual void start() = 0;

  /**
   * Stop the audio interface, i.e., calling the registered callback functions anymore.
   * Pure virtual method, must be implemented in all concrete derived classes.
   */
  virtual void stop() = 0;
};

} // namespace audiointerfaces
} // namespace visr

#endif // #ifndef VISR_LIBRRL_AUDIO_INTERFACE_HPP_INCLUDED
