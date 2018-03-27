/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_AUDIO_CONNECTION_DESCRIPTOR_HPP_INCLUDED
#define VISR_AUDIO_CONNECTION_DESCRIPTOR_HPP_INCLUDED

#include "../channel_list.hpp"
#include "../export_symbols.hpp"

#include <ciso646>
#include <cstddef>
#include <initializer_list>
#include <list>
#include <set>
#include <string>
#include <vector>

namespace visr
{

class AudioPortBase;
class Component;

namespace impl
{
class AudioPortBaseImplementation;

/**
 * Store data from definition in derived class until initialisation of runtime structures.
 */
struct VISR_CORE_LIBRARY_SYMBOL AudioConnection
{
public:
  /**
   * Default constructor, required for use in standard containers.
   * Creates a struct with empty strings for all members.
   */
  AudioConnection()
   : mSender( nullptr )
   , mReceiver( nullptr )
  {
  }

  AudioConnection( AudioPortBaseImplementation * pSender,
                   ChannelList const & pSendIndices,
                   AudioPortBaseImplementation * pReceiver,
                   ChannelList const & pReceiveIndices );

  bool operator<(AudioConnection const & rhs) const;

  AudioPortBaseImplementation * sender() const { return mSender; }
  AudioPortBaseImplementation * receiver() const { return mReceiver; }

  ChannelList const & sendIndices() const { return mSendIndices; }
  ChannelList const & receiveIndices( ) const { return mReceiveIndices; }

private:
  AudioPortBaseImplementation * mSender;
  AudioPortBaseImplementation * mReceiver;
  ChannelList const mSendIndices;
  ChannelList const mReceiveIndices;
};

using AudioConnectionTable = std::multiset< AudioConnection >;

std::ostream & operator<<( std::ostream & str, impl::AudioConnection const & conn );

std::ostream & operator<<( std::ostream & str, impl::AudioConnectionTable const & table );

} // namespace impl
} // namespace visr

#endif // #ifndef VISR_AUDIO_CONNECTION_DESCRIPTOR_HPP_INCLUDED
