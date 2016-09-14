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
// Forward declarations
namespace ril
{
class AtomicComponent;
class Component;
class AudioPort;
}

namespace rrl
{

class AudioSignalDescriptor
{
public:
  using SignalIndexType = std::size_t;

  AudioSignalDescriptor();

  explicit AudioSignalDescriptor( ril::AudioPort const * port, SignalIndexType index );

  /**
  * 'less than operator', used for ordering in a map.
  */
  bool operator<(AudioSignalDescriptor const & rhs) const;

  bool operator==(AudioSignalDescriptor const & rhs) const;

  ril::AudioPort const* mPort;
  SignalIndexType mIndex;

  // with proper C++11 support, this could be instantiated in place (using the constexpr mechanism)
  static SignalIndexType const cInvalidIndex;
};

  /**
  * Format a port name
  */
  std::string portWithComponentName( ril::AudioPort const * port );

  std::string printAudioSignalDescriptor( AudioSignalDescriptor const & desc );

class AudioConnectionMap
{
private:
  struct CompareDescriptors
  {
    bool operator()( AudioSignalDescriptor const & lhs, AudioSignalDescriptor const & rhs ) const
    {
      if( lhs.mPort < rhs.mPort )
      {
        return true;
      }
      else if( lhs.mPort == rhs.mPort )
      {
        return lhs.mIndex < rhs.mIndex;
      }
      return false;
    }
  };

public:
  using Container = std::multimap< AudioSignalDescriptor, AudioSignalDescriptor, CompareDescriptors >;

  using value_type = Container::value_type;

  using const_iterator = Container::const_iterator;

  /**
   * Default constructor, creates an empty connection map.
   */
  AudioConnectionMap();

  explicit AudioConnectionMap( ril::Component const & component,
                               bool recursive = false );

  bool fill( ril::Component const & component,
             std::ostream & messages,
             bool recursive = false );

  Container const & connections() const { return mConnections; }

  const_iterator begin() const { return mConnections.begin(); }

  const_iterator end() const { return mConnections.end(); }

  std::size_t size() const { return mConnections.size(); }

  std::pair<const_iterator, const_iterator > equal_range( AudioSignalDescriptor const signal ) const
  {
    return mConnections.equal_range( signal );
  }

  const_iterator findFirst( AudioSignalDescriptor const signal ) const
  {
    return mConnections.find( signal );
  }

  /**
   * @throw std::invalid_argument if the flow graph is inconsistent.
   * If an exception is thrown, the object is not altered.
   */
  void resolvePlaceholders( AudioConnectionMap const & fullConnections );
private:
  bool fillRecursive( ril::Component const & component,
                      std::ostream & messages,
                      bool recursive );

  Container mConnections;
};

std::ostream & operator<<(std::ostream & stream, AudioConnectionMap const & connections);

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_AUDIO_CONNECTION_MAP_HPP_INCLUDED
