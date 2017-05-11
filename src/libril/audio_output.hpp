/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_AUDIO_OUTPUT_HPP_INCLUDED
#define VISR_AUDIO_OUTPUT_HPP_INCLUDED

#include "audio_port_base.hpp"

#include "constants.hpp" // for the default SampleType
#include "export_symbols.hpp"

#include <stdexcept>

namespace visr
{

/**
 * Common base class for audio output ports, not specific to a sample type
 * This class contains the common implementation of the templated concrete port types.
 */
class VISR_CORE_LIBRARY_SYMBOL AudioOutputBase: public AudioPortBase
{
public:
  /**
   * Constructor.
   * @param name The name of the port as a null-terminated character array. Must be unique among the audio ports of the containing components.
   * @param container The containing component (atomic or composite)
   * @param typeId The sample type id of the contained component.
   * @param width The width of the port, i.e., the number of single audio channels provided.
   */
  /*VISR_CORE_LIBRARY_SYMBOL*/ AudioOutputBase( char const * name, Component & container, AudioSampleType::Id typeId, std::size_t width );

  /**
   * Desctructor (virtual)
   * Virtual because the base class destructor is also virtual.
   */
  /*VISR_CORE_LIBRARY_SYMBOL*/ virtual ~AudioOutputBase() override;
};

/**
 * Class template for concrete output port types holding a specific sample type.
 * @tparam DataType The sample data type used by this port.
 */
template<typename DataType>
class AudioOutputT: public AudioOutputBase
{
public:
  /**
   * Constructor.
   * @param name The name of the port as a null-terminated character array. Must be unique 
   * among the audio ports of the containing component.
   * @param container The containing component (composite or atomic).
   * @param width The width, i.e., number of elementary audio signals provided by this class. Optional parameter, default is zero.
   * The width can be set or modified during the initialisation phase of the containing component.
   */
  AudioOutputT( char const * name, Component & container, std::size_t width = 0 )
    : AudioOutputBase( name, container, AudioSampleType::TypeToId<DataType>::id, width )
  {
  }

  /**
   * Destructor (virtual)
   */
  virtual ~AudioOutputT() override = default;

  /**
   * Return a non-constant pointer to the first sample of the first (technically zeroth) signal of this port.
   * Samples for each channel are stored contiguously, and the pointer addresses of subsequent audio signals 
   * can be calculated by adding multiples of \p channelStrideSamples() to the \p data() pointer.
   */
  DataType * data() { return static_cast<DataType *>(AudioPortBase::basePointer()); }

  /**
   * Return the first sample of a given signal, checked version.
   * @param idx The signal index (zero-offset).
   * @throw std::out_of_range If \p idx exceeds the width of the port.
   */
  DataType * at( std::size_t idx )
    {
      if( idx >= width() )
      {
        throw std::out_of_range( "AudioOutput::at() exceeds port width." );
      }
      return operator[]( idx );
    }

  /**
   * Return the first sample of a given signal, unchecked version.
   * @param idx The signal index (zero-offset).
   */
  DataType * operator[]( std::size_t idx )
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
      DataType * ptr( data() );
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
