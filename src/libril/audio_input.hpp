/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_AUDIO_INPUT_HPP_INCLUDED
#define VISR_AUDIO_INPUT_HPP_INCLUDED

#include "audio_port_base.hpp"

#include "constants.hpp"

namespace visr
{

class AudioInputBase: public AudioPortBase
{
public:
  AudioInputBase( char const * name, Component & container, AudioSampleType::Id typeId, std::size_t width );

  virtual ~AudioInputBase() override;
};

template<typename DataType>
class AudioInputT: public AudioInputBase
{
public:
  AudioInputT( char const * name, Component & container, std::size_t width = 0 )
   : AudioInputBase( name, container, AudioSampleType::TypeToId<DataType>::id, width )
  {}

  virtual ~AudioInputT() override = default;

  std::size_t channelStrideBytes() const { return channelStrideSamples() * sizeof( DataType ); }

  DataType const * base() const { return static_cast<DataType const * >(AudioPortBase::basePointer()); }

  DataType const * at( std::size_t idx ) const
  {
    if( idx >= width() )
    {
      throw std::out_of_range( "AudioInput::at() exceeds port width." );
    }
    return operator[](idx);
  }

  DataType const * operator[]( std::size_t idx ) const
  {
    return base() + idx * channelStrideSamples();
  }

  /**
   * Write the channel pointers of all contained elements to an output iterator.
   * The container that is pointed to by \p outIt must provide space for at least \p width() elements.
   * @tparam OutputIterator a type fulfilling the OutputIterator concept that accepts assignement of \p DataType const * values.
   * @param outIt the output iterator to be written to.
   * @return An output iterator pointing to the element behind the last inserted element.
   */
  template< class OutputIterator >
  OutputIterator getChannelPointers( OutputIterator outIt )
  {
    std::size_t const wd( width() );
    std::size_t const stride( channelStrideSamples() );
    DataType const * ptr( base() );
    for( std::size_t chIdx(0); chIdx < wd; ++chIdx, ptr += stride, ++outIt )
    {
      *outIt = ptr;
    }
    return outIt;
  }
};

using AudioInput = AudioInputT<SampleType>;

} // namespace visr

#endif // #ifndef VISR_AUDIO_INPUT_HPP_INCLUDED
