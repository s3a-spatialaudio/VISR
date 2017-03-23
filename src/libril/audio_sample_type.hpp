/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_AUDIO_SAMPLE_TYPE_HPP_INCLUDED
#define VISR_AUDIO_SAMPLE_TYPE_HPP_INCLUDED

#include "export_symbols.hpp"

#include <cstdint>
#include <cstddef>

#define VISR_AUDIO_SAMPLE_TYPE_DEFINITION( TypeParameter, IdParameter )\
template<> struct TypeToId<TypeParameter> { static constexpr Id id = IdParameter; };\
template<> struct IdToType<IdParameter> { using Type = TypeParameter; };

namespace visr
{

namespace AudioSampleType
{
  /**
   * Enumeration for the different sample types.
   * Can be extended, but the type must be registered with the
   * VISR_AUDIO_SAMPLE_TYPE_DEFINITION macro and in the typeSize() 
   * function in the implementation file.
   */
  enum Id
  {
    floatId = 0,
    doubleId = 1,
    longDoubleId = 2,
    uint8Id = 3,
    int8Id = 4,
    uint16Id = 5,
    int16Id = 6,
    uint32Id = 7,
    int32Id = 8
  };

  /**
   * Compile-time translation from type id to type.
   * Usage: AudioSampleType::IdToType<id>::Type
   */
  template< AudioSampleType::Id id > struct IdToType {};

  /**
   * Compile-time translation from sample type to type id.
   * Usage: AudioSampleType::IdToType<Type>::TypeToId::id
   */
  template< typename > struct TypeToId {};

  /**
   * Return the element size for a type id at runtime.
   */
  VISR_CORE_LIBRARY_SYMBOL std::size_t typeSize( Id id );

  /**
   * Register the translations for the Type<-> conversion macros.
   */
  //@{
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( float, floatId )
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( double, doubleId )
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( long double, longDoubleId )
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( int8_t, int8Id )
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( uint8_t, uint8Id )
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( int16_t, int16Id )
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( uint16_t, uint16Id )
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( int32_t, int32Id )
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( uint32_t, uint32Id )
  //@}
}

} // namespace visr

#endif // #ifndef VISR_AUDIO_SAMPLE_TYPE_HPP_INCLUDED
