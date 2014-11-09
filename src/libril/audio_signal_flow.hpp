/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_LIBRIL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED

#include "audio_interface.hpp"
#include "audio_port.hpp"
#include "constants.hpp"

#include <array>
#include <initializer_list>
#include <map>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <valarray>

namespace visr
{
namespace ril
{

// Forward declaration
template <typename T> class CommunicationArea;
class AudioComponent;
class AudioPort;

class AudioSignalFlow
{
public:
  explicit AudioSignalFlow( std::size_t period, SamplingFrequencyType samplingFrequency );

  ~AudioSignalFlow();

  /**
   * Query whether the signal flow has been set up successfully.
   */
  bool initialised() const { return mInitialised; }


  static void  processFunction( void* /* userData */,
                                SampleType const * const * captureSamples,
                                SampleType * const * playbackSamples,
                                AudioInterface::CallbackResult& callbackResult );

  /**
   * The subclass-dependent code that will be executed within the processFunction() callback.
   */
  virtual void process() = 0;

  /**
   * Method to be implemented by derived subclasses.
   */
  virtual void setup() = 0;

  std::size_t period() const { return mPeriod; }

  CommunicationArea<SampleType>& getCommArea() { return *mCommArea; }

  CommunicationArea<SampleType> const& getCommArea( ) const { return *mCommArea; }

  void initCommArea( std::size_t numberOfSignals, std::size_t signalLength,
                     std::size_t alignmentElements = cVectorAlignmentSamples );

  template< std::size_t vecLength >
  void assignCommunicationIndices( std::string const & componentName,
                                   std::string const & portName,
                                   std::array<AudioPort::SignalIndexType, vecLength > const & indexVector )
  {
    AudioPort & port = findPort( componentName, portName ); // throws an exception if component port does not exist.
    port.assignCommunicationIndices( indexVector );
  }

  void assignCommunicationIndices( std::string const & componentName,
                                   std::string const & portName,
                                   AudioPort::SignalIndexType const * const val, std::size_t vecLength )
  {
    AudioPort & port = findPort( componentName, portName ); // throws an exception if component port does not exist.
    port.assignCommunicationIndices( val, vecLength );
  }

  template<typename IteratorType>
  void assignCommunicationIndices( std::string const & componentName,
                                   std::string const & portName,
                                   IteratorType begin, IteratorType end )
  {
    AudioPort & port = findPort( componentName, portName ); // throws an exception if component port does not exist.
    port.assignCommunicationIndices( begin, end );
  }

  /**
  * Set the indices of a port of an AudioComponent from a brace-enclosed list
  * written directly into the source code.
  * Preferred method for hard-coded signal flows
  */
  void assignCommunicationIndices( std::string const & componentName,
                                   std::string const & portName,
                                   std::initializer_list<AudioPort::SignalIndexType> const & indexVector );

  /**
   * 
   */
  //@{


  template<std::size_t vecLength>
  void assignCaptureIndices( std::array<AudioPort::SignalIndexType, vecLength> const & indexArray )
  {
    assignCaptureIndices( &indexArray[0], &indexArray[0] + indexArray.size() );
  }

  template<std::size_t vecLength>
  void assignPlaybackIndices( std::array<AudioPort::SignalIndexType, vecLength> const & indexArray )
  {
    assignPlaybackIndices( &indexArray[0], &indexArray[0] + indexArray.size( ) );
  }

  void assignCaptureIndices( std::initializer_list<AudioPort::SignalIndexType> const & indexVector );

  void assignPlaybackIndices( std::initializer_list<AudioPort::SignalIndexType> const & indexVector );

  void assignCaptureIndices( AudioPort::SignalIndexType const * indexArrayPtr, std::size_t vecLength );

  void assignPlaybackIndices( AudioPort::SignalIndexType const * indexArrayPtr, std::size_t vecLength );

  //@}

  /**
   *
   */
  bool checkSignalFlow( std::stringstream & messages ) const;

  /**
   * Mark the signal flow as "initialised".
   * @todo Decide whether this is the reight place for a consistency check.
   */
  void setInitialised( bool newState = true ) { mInitialised = newState; }

  /**
   * Register a
   * @throw std::invalid_argument If the component could not be inserted, e.g., if a component with this name already exists.
   */
  void registerComponent( AudioComponent * component, char const * componentName );

  /**
   * Return the number of input channels for this signal flow.
   * @throw If this function is called while the signal is not initialised.
   */
  std::size_t numberOfCaptureChannels() const
  {
    return mCaptureIndices.size();
  }

  /**
  * Return the number of output channels for this signal flow.
  * @throw If this function is called while the signal is not initialised.
  */
  std::size_t numberOfPlaybackChannels( ) const
  {
    return mPlaybackIndices.size( );
  }

protected:
  /**
   * Functions to called by derived signal flow classes to setup the members in this class.
   */
  //@{
  //@}

private:
  /**/
  void processInternal( SampleType const * const * captureSamples,
                        SampleType * const * playbackSamples,
                        AudioInterface::CallbackResult& callbackResult );

  /**
   * find a port of a specific audio component, both specified by name
   * @throw std::invalid_argument If either component or port specified by the respective name does not exist.
   */
  AudioPort & findPort( std::string const & componentName,
                        std::string const & portName );

  bool mInitialised;

  std::unique_ptr<CommunicationArea<SampleType> > mCommArea;

  /**
   * The number of samples processed in one iteration of the signal flow graph.
   */
  std::size_t const mPeriod;

  SamplingFrequencyType const mSamplingFrequency;

  /**
   * Type for collecion and lookup of all audio components contained in this signal flow.
   * @note: This list does not assume ownership of the components.
   */
  using ComponentTable = std::map<std::string, AudioComponent*>;

  ComponentTable mComponents;

  std::vector<AudioPort::SignalIndexType> mCaptureIndices;
  std::vector<AudioPort::SignalIndexType> mPlaybackIndices;
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED