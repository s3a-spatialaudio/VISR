/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_AUDIO_PORT_BASE_HPP_INCLUDED
#define VISR_AUDIO_PORT_BASE_HPP_INCLUDED

#include "port_base.hpp" // For 'Direction' enum
#include "audio_sample_type.hpp"
#include "export_symbols.hpp"

#include <string>

namespace visr
{
// Forward declaration(s)
class Component;

namespace impl
{
class AudioPortBaseImplementation;
}

class VISR_CORE_LIBRARY_SYMBOL AudioPortBase
{
public:
  explicit AudioPortBase( std::string const & name, Component & container, AudioSampleType::Id sampleType, PortBase::Direction direction );

  explicit AudioPortBase( std::string const & name, Component& container, AudioSampleType::Id sampleType, PortBase::Direction direction, std::size_t width );

  virtual ~AudioPortBase();

  void setWidth( std::size_t newWidth );

  std::size_t width() const noexcept;

  /**
   * Return the alignment of the channel vectors in bytes.
   * This method can be called at any point of the lifetime of an audio port, and the alignemnt is guaranteed not to change.
   */
  std::size_t alignmentBytes() noexcept;

  /**
  * Return the guaranteed alignment of the channel vectors (in multiples of the sample size).
  * This function can be called at any point of the audio port's lifetime and the alignment remains constant througout that lifetime.
  */
  std::size_t alignmentSamples() noexcept;

  std::size_t channelStrideSamples() const noexcept;

  std::size_t channelStrideBytes() const noexcept;

  AudioSampleType::Id sampleType() const noexcept;

  std::size_t sampleSize() const noexcept;

  /**
   * Access the opaque implementation object.
   * @todo: Limit access if possible at all.
   */
  //@{
  impl::AudioPortBaseImplementation & implementation();

  impl::AudioPortBaseImplementation const & implementation() const;
  //@}

protected:
  void * basePointer();

  void const * basePointer() const;

private:
  impl::AudioPortBaseImplementation* mImpl;
};

} // namespace visr

#endif // #ifndef VISR_AUDIO_PORT_BASE_HPP_INCLUDED
