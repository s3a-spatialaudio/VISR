/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_AUDIO_INTERFACE_HPP_INCLUDED
#define VISR_LIBRRL_AUDIO_INTERFACE_HPP_INCLUDED

#include <cstddef>
#include <string>

namespace visr
{
namespace rrl
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
//  struct SampleLayout
//  {
//    enum Type
//    {
//      Contiguous,
//      Interleaved,
//      Automatic
//    };
//  };

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
    std::size_t numPlayChannels( ) const { return mNumPlaybackChannels; }
    SampleRateType sampleRate() const { return mSampleRate; }
    std::size_t periodSize( ) const { return mPeriodSize; }
//    SampleLayout::Type sampleLayout() const { return mSampleLayout; }

    //@}
  private:
    std::size_t const mNumCaptureChannels;
    std::size_t const mNumPlaybackChannels;
    SampleRateType const mSampleRate;
    std::size_t const mPeriodSize;
//    SampleLayout::Type mSampleLayout;
  };


  /// Preliminary definition of sample types
  //@{
  using ExternalSampleType = float;

  /// note: the internal sample type should be of no concern to the abstract interface.
  using InternalSampleType = float;
  //@}

  /** 
<<<<<<< HEAD:src/libril/audio_interface.hpp
   * Status returned by the callback initialised by the audio interface
=======
   * Status returned by the callback initialed by the audio interface
>>>>>>> 3b8f1889c6e66d81bd3c390bfd7f720dbfdacaa7:src/librrl/audio_interface.hpp
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

//  virtual void parseSpecificConf( std::string const & config ) = 0;
    
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

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_AUDIO_INTERFACE_HPP_INCLUDED
