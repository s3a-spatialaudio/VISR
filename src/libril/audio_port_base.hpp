/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_AUDIO_PORT_BASE_HPP_INCLUDED
#define VISR_AUDIO_PORT_BASE_HPP_INCLUDED

#include "port_base.hpp" // For 'Direction' enum
#include "audio_sample_type.hpp"

#include <memory>
#include <string>


namespace visr
{
// Forward declaration(s)
class Component;

namespace impl
{
class AudioPortBaseImplementation;
}

class AudioPortBase
{
public:
  explicit AudioPortBase( std::string const & name, Component & container, AudioSampleType::Id sampleType, PortBase::Direction direction );

  explicit AudioPortBase( std::string const & name, Component& container, AudioSampleType::Id sampleType, PortBase::Direction direction, std::size_t width );

  virtual ~AudioPortBase();

  void setWidth( std::size_t newWidth );

  std::size_t width() const noexcept;

  std::size_t channelStrideSamples() const noexcept;

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
  std::unique_ptr<impl::AudioPortBaseImplementation> mImpl;
};

} // namespace visr

#endif // #ifndef VISR_AUDIO_PORT_BASE_HPP_INCLUDED
