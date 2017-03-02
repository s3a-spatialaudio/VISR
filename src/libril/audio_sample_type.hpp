/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_AUDIO_SAMPLE_TYPE_HPP_INCLUDED
#define VISR_AUDIO_SAMPLE_TYPE_HPP_INCLUDED

#include "component.hpp"
#include "processable_interface.hpp"

#include <cstddef>

#define VISR_AUDIO_SAMPLE_TYPE_DEFINITION( TypeParameter, IdParameter )\
template<> struct TypeToId<TypeParameter> { static constexpr Id id = IdParameter; };\
template<> struct IdToType<IdParameter> { using Type = TypeParameter; };

namespace visr
{

/**
 * Maybe move that here:
 * Definition for the default sample type.
 */
// using SampleType = float;

struct AudioSampleType
{
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

  template< AudioSampleType::Id id > struct IdToType {};
  template< typename > struct TypeToId {};

  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( float, floatId );
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( double, doubleId );
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( int8_t, int8Id );
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( uint8_t, uint8Id );
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( int16_t, int16Id );
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( uint16_t, uint16Id );
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( int32_t, int32Id );
  VISR_AUDIO_SAMPLE_TYPE_DEFINITION( uint32_t, uint32Id );
};

} // namespace visr

#endif // #ifndef VISR_AUDIO_SAMPLE_TYPE_HPP_INCLUDED
