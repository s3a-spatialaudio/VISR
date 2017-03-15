/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_AUDIO_CONNECTION_MAP_HPP_INCLUDED
#define VISR_LIBRRL_AUDIO_CONNECTION_MAP_HPP_INCLUDED

#include <libril/constants.hpp>

#include <iosfwd>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace impl
{
class AudioPortBaseImplementation;
class ComponentImplementation;
}

namespace rrl
{

/**
 * Data structure representing a single audio channel.
 */
class AudioChannel
{
public:
  explicit AudioChannel( impl::AudioPortBaseImplementation const * port, std::size_t channel )
    : mVal( port, channel )
  {
  }
  impl::AudioPortBaseImplementation const * port() const { return std::get<0>( mVal ); }
  std::size_t const channel() const { return std::get<1>( mVal ); }

  AudioChannel( AudioChannel const & rhs ) = default;

  AudioChannel& operator=( AudioChannel const & rhs ) = default;

  AudioChannel( AudioChannel && rhs ) = default;

  AudioChannel& operator=( AudioChannel && rhs ) = default;


  bool operator<( AudioChannel const & rhs ) const { return mVal < rhs.mVal; }

  bool operator==( AudioChannel const & rhs ) const { return mVal == rhs.mVal; }

private:
  std::tuple<impl::AudioPortBaseImplementation const *, std::size_t> mVal;
};

/**
 * Stream operator to print an audio channel.
 * Format <full port name>:<channel index>
 */
std::ostream& operator<<( std::ostream & str, AudioChannel const & channel );

class AudioConnectionMap
{
public:

  using Container = std::multimap< AudioChannel, AudioChannel >;

  using ValueType = Container::value_type;

  using iterator = Container::const_iterator;
  using const_iterator = Container::const_iterator;

  /**
  * Default constructor, creates an empty connection map.
  */
  AudioConnectionMap();

  explicit AudioConnectionMap( impl::ComponentImplementation const & component,
                               bool recursive = false );

  bool fill( impl::ComponentImplementation const & component,
             std::ostream & messages,
             bool recursive = false );

  void insert( ValueType const & connection );

  void insert( AudioChannel const & sender, AudioChannel const & receiver );

  Container const & connections() const { return mConnections; }

  const_iterator begin() const { return mConnections.begin(); }

  const_iterator end() const { return mConnections.end(); }

  std::size_t size() const { return mConnections.size(); }

  std::pair<const_iterator, const_iterator > connectionsForReceiveChannel( AudioChannel const & audioChannel ) const
  {
    return mConnections.equal_range( audioChannel );
  }
  /**
   */
  const_iterator findReceiveChannel( AudioChannel const & signal ) const;

  AudioConnectionMap resolvePlaceholders( ) const;

private:
  bool fillRecursive( impl::ComponentImplementation const & component,
                      std::ostream & messages,
                      bool recursive );

  Container mConnections;
};

/**
 * Stream operator to print an audio connection map.
 * Output format: the contained connection entries in the format 
 * <send audio channel>-><receive audio channel> separated by line ends.
 */
std::ostream & operator<<( std::ostream & stream, AudioConnectionMap const & connections );

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_AUDIO_CONNECTION_MAP_HPP_INCLUDED
