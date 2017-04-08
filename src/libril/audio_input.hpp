/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_AUDIO_INPUT_HPP_INCLUDED
#define VISR_AUDIO_INPUT_HPP_INCLUDED

#include "audio_port_base.hpp"

#include "constants.hpp"
#include "export_symbols.hpp"

#include <stdexcept>

namespace visr
{

/**
 * Base class for audio input ports.
 * This base class is not intended to be used by API users.
 * This class itself cannot be instantiated, because it is not 
 * associated with a specific sample type. Only derived classes may actually be instantiated.
 */
class AudioInputBase: public AudioPortBase
{
protected:
  /**
   * Constructor (protected). Sets up the data members of the base port type and defines it as an input.
   * @param name The name of the port as a zero-terminated character array. Name must be unique among the audio ports of the containing component.
   * @param container The component (atomic or composite) holding that audio type.
   * @param typeId An enumeration value that specifies the data type.
   * @param width The number of single audio channels associated with this port.
   */
  VISR_CORE_LIBRARY_SYMBOL AudioInputBase( char const * name, Component & container, AudioSampleType::Id typeId, std::size_t width );
public:
  /**
   * Destructor (virtual).
   * @note Reconsider whether audio ports shall be instantiated polymorphically. Otherwise, the destructor would not need to  be virtual.
   */
  VISR_CORE_LIBRARY_SYMBOL virtual ~AudioInputBase() override;
};

/**
 * Class template for concrete audio inputs holding samples of a specific type.
 * @tparam DataType The sample type used by this audio port type.
 */
template<typename DataType>
class AudioInputT: public AudioInputBase
{
public:
  /**
   * Constructor, construct an audio input port of a specific sample type.
   * @param name The name of the input port. Zer-terminated character array that must be unique among the audio ports of the containing components.
   * @param container The component to be holding this audio port
   * @param width The port width, i.e., number of single audio signals, represented by this port. Optional parameter, default value 0.
   */
  AudioInputT( char const * name, Component & container, std::size_t width = 0 )
   : AudioInputBase( name, container, AudioSampleType::TypeToId<DataType>::id, width )
  {}

  /**
   * Destructor (virtual)
   * @todo decide whether the destructor must be virtual, i.e., whether the class is instantiated polymorphically.
   */
  virtual ~AudioInputT() override = default;

  /**
   * Return the base pointer of the input sample.
   * This is the pointer to the first (index 0) channel signal.
   */
  DataType const * data() const { return static_cast<DataType const * >(AudioPortBase::basePointer()); }

  /**
   * Return the pointer to element 0 of the \p idx - th channel of the audio port, with checking
   * @param idx The zero-offset index of the requested audio channel.
   * @throw std::out_of_range if \p idx exceeds the admissible channel indices, i.e., idx >= width()
   */
  DataType const * at( std::size_t idx ) const
  {
    if( idx >= width() )
    {
      throw std::out_of_range( "AudioInput::at() exceeds port width." );
    }
    return operator[](idx);
  }

  /**
  * Return the pointer to element 0 of the \p idx - th channel of the audio port, unchecked version.
  * If the index \p idx exceeds the admissible range, the result is undefined.
  * @param idx The zero-offset index of the requested audio channel.
  */
  DataType const * operator[]( std::size_t idx ) const
  {
    return data() + idx * channelStrideSamples();
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
    DataType const * ptr( data() );
    for( std::size_t chIdx(0); chIdx < wd; ++chIdx, ptr += stride, ++outIt )
    {
      *outIt = ptr;
    }
    return outIt;
  }
};

/**
 * Alias for audio input ports using the default datatype (typically float)
 */
using AudioInput = AudioInputT<SampleType>;

} // namespace visr

#endif // #ifndef VISR_AUDIO_INPUT_HPP_INCLUDED
