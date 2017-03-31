/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_TYPE_HPP_INCLUDED
#define VISR_PARAMETER_TYPE_HPP_INCLUDED

#include "detail/compile_time_hash_fnv1.hpp"

#include <cstddef>

namespace visr
{

using ParameterType = uint64_t;

//enum class ParameterType
//{
//  MatrixFloat,
//  MatrixDouble,
//  MatrixFloatComplex,
//  MatrixDoubleComplex,
//  BiquadMatrixFloat,
//  BiquadMatrixDouble,
//  IndexedString, /**< Combination of an index and a string. */
//  IndexedVectorFloat,
//  IndexedVectorDouble,
//  ListenerPosition,
//  ObjectVector,
//  SignalRouting,
//  String, // TODO: We need a proper string type
//  TimeFrequencyFloat,
//  TimeFrequencyDouble,
//  VectorFloat,
//  VectorDouble
//};

/**
 * Metaprogramming construct to translate a type to its corresponding ID.
 */
template< typename Parameter >
struct ParameterToId {};

template< ParameterType Id >
struct IdToParameter;

/**
 * Compile-time translation from parameters to the corresponding parameter config type
 */
template< typename Parameter >
struct ParameterToConfigType {};

} // namespace visr

#define DEFINE_PARAMETER_TYPE( ParameterClassType, ParameterId, ParameterConfigType)\
namespace visr { \
template<> \
struct ParameterToId< ParameterClassType > \
{\
public:\
  static constexpr ParameterType id =  ParameterId;\
  using ConfigType = ParameterConfigType; \
};\
  template<> \
struct IdToParameter< ParameterId > \
{ \
public: \
  using Type = ParameterClassType; \
}; \
  template<> \
struct ParameterToConfigType< ParameterClassType > \
{\
public:\
  using ConfigType = ParameterConfigType;\
}; \
}

#endif // #ifndef VISR_PARAMETER_TYPE_HPP_INCLUDED
