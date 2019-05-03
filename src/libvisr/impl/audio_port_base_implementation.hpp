/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_IMPL_AUDIO_PORT_BASE_IMPLEMENTATION_HPP_INCLUDED
#define VISR_IMPL_AUDIO_PORT_BASE_IMPLEMENTATION_HPP_INCLUDED

#include "port_base_implementation.hpp"

#include "../audio_sample_type.hpp"
#include "../export_symbols.hpp"

//#include <cstddef>
//#include <exception>
//#include <iterator>
//#include <limits>
#include <string>
//#include <vector>

#include <valarray>

namespace visr
{
class AudioPortBase;

namespace impl
{

class AudioPortBaseImplementation: public PortBaseImplementation
{
public:

  VISR_CORE_LIBRARY_SYMBOL explicit AudioPortBaseImplementation( char const * name, AudioPortBase& containingPort, ComponentImplementation * container, AudioSampleType::Id sampleType, visr::PortBase::Direction direction );

  VISR_CORE_LIBRARY_SYMBOL explicit AudioPortBaseImplementation( char const * name, AudioPortBase& containingPort, ComponentImplementation* container, AudioSampleType::Id sampleType, visr::PortBase::Direction direction, std::size_t width );

  VISR_CORE_LIBRARY_SYMBOL virtual ~AudioPortBaseImplementation();

  VISR_CORE_LIBRARY_SYMBOL AudioPortBase & containingPort();

  VISR_CORE_LIBRARY_SYMBOL AudioPortBase const & containingPort() const;

  /**
   * Query whether the port is initialised, e.g., whether a base address and a channel stride has been assigned.
   */
  VISR_CORE_LIBRARY_SYMBOL bool initialised() const noexcept;

  /**
   * Reset the port into uninitialized state, that is, reset the base pointer and channel stride to their initial settings
   */
  VISR_CORE_LIBRARY_SYMBOL void reset() noexcept;

  /**
   * Set the width, i.e., number of single audio channels, for this port.
   * @throw std::logic_error if called when in initialised state.
   */
  VISR_CORE_LIBRARY_SYMBOL void setWidth( std::size_t newWidth );

  /**
   * Return the number of single audio channels allocated for this port
   */
  VISR_CORE_LIBRARY_SYMBOL std::size_t width() const noexcept;

  /**
  * Return the alignment of the channel vectors in bytes.
  * This method can be called at any point of the lifetime of an audio port, and the alignemnt is guaranteed not to change.
  */
  VISR_CORE_LIBRARY_SYMBOL std::size_t alignmentBytes() noexcept;

  /**
  * Return the guaranteed alignment of the channel vectors (in multiples of the element size).
  * This function can be called at any point of the audio port's lifetime and the alignment remians constant througout that lifetime.
  */
  VISR_CORE_LIBRARY_SYMBOL std::size_t alignmentSamples() noexcept;

  /**
   * Return the number of samples between the vectors of successive audio channels.
   * If the port has not been initialised, 0 is returned.
   */
  VISR_CORE_LIBRARY_SYMBOL std::size_t channelStrideSamples() const noexcept;

  VISR_CORE_LIBRARY_SYMBOL std::size_t channelStrideBytes() const noexcept;

  //@}

  VISR_CORE_LIBRARY_SYMBOL void setChannelStrideSamples(std::size_t stride);

  VISR_CORE_LIBRARY_SYMBOL AudioSampleType::Id sampleType() const noexcept;

  VISR_CORE_LIBRARY_SYMBOL std::size_t sampleSize() const noexcept;

  VISR_CORE_LIBRARY_SYMBOL void setBasePointer( void * base );

  VISR_CORE_LIBRARY_SYMBOL void setBufferConfig( void * base, std::size_t channelStrideSamples );

  VISR_CORE_LIBRARY_SYMBOL void const * basePointer() const;

  VISR_CORE_LIBRARY_SYMBOL void * basePointer();
protected:
  AudioPortBase & mContainingPort;

  AudioSampleType::Id const cSampleType;

  std::size_t const cSampleSize;

  std::size_t mWidth;

  void * mBasePointer;

  std::size_t mChannelStrideSamples;
};

} // namespace impl
} // namespace visr

#endif // #ifndef VISR_IMPL_AUDIO_PORT_BASE_IMPLEMENTATION_HPP_INCLUDED
