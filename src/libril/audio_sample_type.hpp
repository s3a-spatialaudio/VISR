/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_AUDIO_SAMPLE_TYPE_HPP_INCLUDED
#define VISR_AUDIO_SAMPLE_TYPE_HPP_INCLUDED

#include "export_symbols.hpp"

#include <complex>
#include <cstdint>
#include <cstddef>

/**
 * Macro to ease the registration of compile-time translation between sample types and the type ids
 * @param TypeParameter The data type, e.g., float
 * @param IdParameter The sample type id, e.g., floatId
 */
#define VISR_AUDIO_SAMPLE_TYPE_DEFINITION( TypeParameter, IdParameter )\
template<> struct TypeToId<TypeParameter> { static constexpr Id id = IdParameter; };\
template<> struct IdToType<IdParameter> { using Type = TypeParameter; };

namespace visr
{

/**
 * Namespace that encapsulates functionality about the different types of audio samples.
 */
namespace AudioSampleType
{
  /**
   * Enumeration for the different sample types.
   * This list can be extended, but the type must be registered with the
   * VISR_AUDIO_SAMPLE_TYPE_DEFINITION macro and in the typeSize() 
   * function in the implementation file.
   */
  enum Id
  {
    floatId = 0,         /**< 32-bit single-precision floating-point data */
    doubleId = 1,        /**< 64-bit double-precision floating-point data */
    longDoubleId = 2,    /**< Extended-precision floating-point data */
    uint8Id = 3,         /**< Unsigned 8-bit integer sample types */
    int8Id = 4,          /**< Signed 8-bit integer sample types */
    uint16Id = 5,        /**< Unsigned 16-bit integer sample types */
    int16Id = 6,         /**< Signed 16-bit integer sample types */
    uint32Id = 7,        /**< Unsigned 32-bit integer sample types */
    int32Id = 8,         /**< Signed 32-bit integer sample types */
    complexFloatId = 9,  /**< Complex data containing 32-bit single-precision floating-point values */
    complexDoubleId = 10 /**< Complex data containing 64-bit double-precision floating-point values */
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
   * Runtime function to query the element size for a type id at runtime.
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
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( std::complex<float>, complexFloatId )
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( std::complex<double>, complexDoubleId )
  //@}
}

} // namespace visr

#endif // #ifndef VISR_AUDIO_SAMPLE_TYPE_HPP_INCLUDED
