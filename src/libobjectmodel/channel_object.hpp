/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_CHANNEL_OBJECT_HPP_INCLUDED
#define VISR_OBJECTMODEL_CHANNEL_OBJECT_HPP_INCLUDED

#include "object.hpp"

#include "export_symbols.hpp"

#include <vector>

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class VISR_OBJECTMODEL_LIBRARY_SYMBOL ChannelObject: public Object
{
public:
  /**
   * The data type used to represent the channels to which the contained audio signals are routed.
   * This corresponds to the loudspeaker IDs used in the array configuration file, i.e., a string.
   */
  using OutputChannelId = std::string;

  /**
   * Container for all output loudspeakers (as loudspeaker labels) of one signal channel.
   */
  using  OutputChannelList = std::vector<OutputChannelId>;

  /**
   * Container type for holding the output channel indices.
   * Each entry marks all output channels to which a specific input is routed.
   */
  using OutputChannelContainer = std::vector< OutputChannelList >;

  ChannelObject( ) = delete;

  explicit ChannelObject( ObjectId id );

  virtual ~ChannelObject();

  /*virtual*/ ObjectTypeId type() const;

  /*virtual*/ std::unique_ptr<Object> clone() const;

  /**
   * Return the number of channels.
   */
  std::size_t size() const;

  /**
   * Return the vector of output channel indices.
   * The number of elements is identical to the number of channels (numberOfChannels())
   */
  OutputChannelContainer const & outputChannels() const;

  /**
   * Return all output loudspeaker indices for one channel.
   * @throw std::out_of_range If \p index exceeds the number of signal channels of the channel object
   */
  OutputChannelList const & outputChannel(std::size_t index) const;

  /**
   * Set the output channel for a specific channel index.
   * @param newChannels Container containing the new output channel indices
   */
  void setOutputChannels(OutputChannelContainer const & newChannels);

  /*
   * Doesn't make sense anymore.
   */
  void setOutputChannels( OutputChannelId const * val, std::size_t numValues );

  /**
   * Set a specific channel to a single output channel.
   */
  void setOutputChannel(std::size_t index, OutputChannelId val);

  /**
   * Set a list of output channels for a 
   */
  void setOutputChannel( std::size_t index, OutputChannelList const & outputChannels );

  LevelType diffuseness() const;

  void setDiffuseness( LevelType newDiffuseness );
protected:

private:
  OutputChannelContainer mOutputChannels;

  LevelType mDiffuseness;

};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_CHANNEL_OBJECT_HPP_INCLUDED
