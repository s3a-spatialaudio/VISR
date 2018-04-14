/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "channel_object.hpp"

#include <algorithm>
#include <exception>

namespace visr
{
namespace objectmodel
{

ChannelObject::ChannelObject( ObjectId id )
 : Object( id )
{
}

/*virtual*/ ChannelObject::~ChannelObject() = default;

/*virtual*/ ObjectTypeId
ChannelObject::type() const
{
  return ObjectTypeId::ChannelObject;
}

/*virtual*/ std::unique_ptr<Object>
ChannelObject::clone() const
{
  return std::unique_ptr<Object>( new ChannelObject( *this ) );
}

std::size_t ChannelObject::size() const
{
  return mOutputChannels.size();
}

ChannelObject::OutputChannelContainer const & ChannelObject::outputChannels() const
{
  return mOutputChannels;
}

ChannelObject::OutputChannelList const & ChannelObject::outputChannel(std::size_t index) const
{
  if (index >= mOutputChannels.size())
  {
    throw std::out_of_range( "Index exceeds number of output channels." );
  }
  return mOutputChannels[index];
}

void ChannelObject::setOutputChannels(OutputChannelContainer const & newChannels)
{
  if( newChannels.size() != numberOfChannels())
  {
    throw std::invalid_argument( "Size of new output channel vector differs from the number of channels of this object." );
  }
  mOutputChannels = newChannels;
}

void ChannelObject::setOutputChannels(OutputChannelId const * val, std::size_t numValues)
{
  if( numValues != numberOfChannels())
  {
    throw std::invalid_argument("Size of new output channel vector differs from the number of channels of this object.");
  }
  // It is possible that the object was inconsistent beforehand, so a resize might be necessary.
  // TODO: Devise proper way to enforce class invariants between the channel count set by the base class and the output channel vector of this object.
  // TODO: Same problem for HOA objects.
  mOutputChannels.resize(numValues);
  // TODO: Define semanticvs (or remove method altogether.
  // std::copy( val, val + numValues, OutputChannelsPerInput( 1, mOutputChannels.begin() ) );
}

void ChannelObject::setOutputChannel(std::size_t index, OutputChannelId val)
{
  if (index >= mOutputChannels.size())
  {
    throw std::out_of_range("Index exceeds number of output channels.");
  }
  mOutputChannels[index] = OutputChannelList( 1, val );
}

void ChannelObject::setOutputChannel( std::size_t index, OutputChannelList const & outputChannels )
{
  if( index >= mOutputChannels.size() )
  {
    throw std::out_of_range( "Index exceeds number of output channels." );
  }
  mOutputChannels[index] = outputChannels;
}

LevelType ChannelObject::diffuseness() const
{
  return mDiffuseness;
}

void ChannelObject::setDiffuseness( LevelType newDiffuseness )
{
  mDiffuseness = newDiffuseness;
}

} // namespace objectmodel
} // namespace visr
