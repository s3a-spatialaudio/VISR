/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_LIBRIL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED

namespace visr
{
namespace ril
{

// Forward declaration
template <typemame T> class CommunicationArea;

class AudioSignalFlow
{
public:
  using SampleType = float;

  AudioSignalFlow();

  ~AudioSignalFlow();

  virtual void setup() = 0;

  virtual void process() = 0;

  bool initialized() const { return mInitialized; }

protected:
  /**
   * Functions to called by derived signal flow classes to setup the members in this class.
   */
  //@{

  //@}



private:
  bool mInitialized;

  std::unique_ptr<CommunicationArea<SampleType> > mCommArea;

};

}
}

#endif // #ifndef VISR_LIBRIL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED