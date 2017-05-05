/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_CHANNEL_OBJECT_HPP_INCLUDED
#define VISR_OBJECTMODEL_CHANNEL_OBJECT_HPP_INCLUDED

#include "object.hpp"

#include <vector>

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class ChannelObject: public Object
{
public:
  /**
   * The data type used to represent the channels to which the contained audio signals are routed.
   * At the moment, this is an unsigned int (same as for the loudspeaker ids in the panning configuration, 
   * but this could be changed to another type (string label?)
   */
  using OutputChannelId = std::size_t;

  /**
   * Container type for holding the output channel indices.
   */
  using OutputChannelContainer = std::vector<OutputChannelId>;

  ChannelObject( );

  explicit ChannelObject( ObjectId id );

  virtual ~ChannelObject();

  /*virtual*/ ObjectTypeId type() const;

  /*virtual*/ std::unique_ptr<Object> clone() const;

  /**
   * Return the vector of output channel indices.
   * The number of elements is identical to the number of channels (numberOfChannels())
   */
  OutputChannelContainer const & outputChannels() const;

  OutputChannelId outputChannel(std::size_t index) const;

  /**
   * Set the output channel for a specific channel index.
   * @param newChannels Container containing the new output channel indices
   */
  void setOutputChannels(OutputChannelContainer const & newChannels);

  void setOutputChannels( OutputChannelId const * val, std::size_t numValues );

  void setOutputChannel(std::size_t index, OutputChannelId val);

protected:

private:
  OutputChannelContainer mOutputChannels;

};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_CHANNEL_OBJECT_HPP_INCLUDED
