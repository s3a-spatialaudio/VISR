/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_AUDIO_INTERFACE_HPP_INCLUDED
#define VISR_LIBRIL_AUDIO_INTERFACE_HPP_INCLUDED

#include <cstddef>

namespace visr
{
namespace ril
{

/**
 * Abstract interface for various audio interfaces.
 */
class AudioInterface
{
public:
  /**
   * Enumeration type to specify how the samples are arranged by the soundcard.
   * This is a pure internal of the specific audio interface used and not needed in other part of 
   * the rendering system
   */
  struct SampleLayout
  {
    enum Type
    {
      Contiguous,
      Interleaved,
      Automatic
    };
  };

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
                            SampleLayout::Type = SampleLayout::Automatic );

    /**
     * Access methods to the data members
     */
    //@{
    std::size_t numCaptureChannels() const { return mNumCaptureChannels; }
    std::size_t numPlayChannels( ) const { return mNumCaptureChannels; }
    SampleRateType sampleRate() const { return mSampleRate; }
    SampleLayout::Type sampleLayout() const { return mSampleLayout; }

    //@}
  private:
    std::size_t mNumCaptureChannels;
    std::size_t mNumPlaybackChannels;
    SampleRateType mSampleRate;
    SampleLayout::Type mSampleLayout;
  };


  /// Preliminary definition of sample types
  //@{
  using ExternalSampleType = float;

  /// note: the internal sample type should be of no concern to the abstract interface.
  using InternalSampleType = float;
  //@}

  /** 
   * Status returned by the callback initialed by the audio interface
   * Maybe we replace this by an enumeration later 
   */
  using CallbackResult = int;

  /**
   * The type of the callback function that can be registered to be called if sufficient data is available 
  */
  typedef void ( *AudioCallback )( void* /* userData */,
                                 ExternalSampleType const * const * /*captureBuffer*/,
                                 ExternalSampleType * const * /*playbackBuffer*/,
                                 CallbackResult& /*errorCode*/);

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

}
}

#endif // #ifndef VISR_LIBRIL_AUDIO_INTERFACE_HPP_INCLUDED
