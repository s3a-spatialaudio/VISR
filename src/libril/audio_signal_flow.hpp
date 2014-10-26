/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_LIBRIL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED

#include "audio_interface.hpp"
#include "audio_port.hpp"
#include "constants.hpp"

#include <array>
#include <map>
#include <memory>
#include <stdexcept>
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
  AudioSignalFlow();

  ~AudioSignalFlow();

  /**
   * Query whether the signal flow has been set up successfully.
   */
  bool initialised() const { return mInitialised; }


  static void  processFunction( void* /* userData */,
                                AudioInterface::ExternalSampleType const * const * captureSamples,
                                AudioInterface::ExternalSampleType * const * playbackSamples,
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

  void setPeriod( std::size_t periodLength );

  CommunicationArea<SampleType>& getCommArea() { return *mCommArea; }

  CommunicationArea<SampleType> const& getCommArea( ) const { return *mCommArea; }

  void initCommArea( std::size_t numberOfSignals, std::size_t signalLength,
                     std::size_t alignmentElements /* = 0 */ );

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

  void setInitialised( bool newState = true ) { mInitialised = newState; }

  /**
   * @throw std::invalid_argument If the compontn could not be inserted, e.g., if it already exists.
   */
  void registerComponent( AudioComponent * component, char const * componentName );

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
  std::size_t mPeriod;

  /**
   * Type for collecion and lookup of all audio components contained in this signal flow.
   * @note: This list does not assume ownership of the components.
   */
  using ComponentTable = std::map<std::string, AudioComponent*>;

  ComponentTable mComponents;

};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED