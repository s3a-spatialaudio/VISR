/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_AUDIO_OUTPUT_HPP_INCLUDED
#define VISR_AUDIO_OUTPUT_HPP_INCLUDED

#include "audio_port_base.hpp"

#include "constants.hpp" // for the default SampleType
#include "export_symbols.hpp"

#include <stdexcept>

namespace visr
{
  class VISR_CORE_LIBRARY_SYMBOL AudioOutputBase: public AudioPortBase
  {
  public:
    AudioOutputBase( char const * name, Component & container, AudioSampleType::Id typeId, std::size_t width );

    virtual ~AudioOutputBase() override;
  };

  template<typename DataType>
  class AudioOutputT: public AudioOutputBase
  {
  public:
    AudioOutputT( char const * name, Component & container, std::size_t width = 0 )
      : AudioOutputBase( name, container, AudioSampleType::TypeToId<DataType>::id, width )
    {
    }

    virtual ~AudioOutputT() override = default;

    DataType * base() { return static_cast<DataType * >(AudioPortBase::basePointer()); }

    DataType * at( std::size_t idx )
    {
      if( idx >= width() )
      {
        throw std::out_of_range( "AudioOutput::at() exceeds port width." );
      }
      return operator[]( idx );
    }

    DataType * operator[]( std::size_t idx )
    {
      // TODO: This is not working properly!
      // return static_cast<DataType * >(base());
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
      DataType * ptr( base() );
      for( std::size_t chIdx( 0 ); chIdx < wd; ++chIdx, ptr += stride, ++outIt )
      {
        *outIt = ptr;
      }
      return outIt;
    }
  };

  /**
   * Non-templated audio output using the default sample type
   */
  using AudioOutput = AudioOutputT<SampleType>;

} // namespace visr

#endif // #ifndef VISR_AUDIO_OUTPUT_HPP_INCLUDED
