/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_PARAMETER_TYPE_HPP_INCLUDED
#define VISR_LIBRIL_PARAMETER_TYPE_HPP_INCLUDED

namespace visr
{
namespace ril
{

enum class ParameterType
{
  MatrixFloat,
  MatrixDouble,
  VectorFloat,
  BiquadMatrixFloat,
  ObjectVector,
  String // TODO: We need a proper string type 
};

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

} // namespace ril
} // namespace visr

#define DEFINE_PARAMETER_TYPE( ParameterClassType, ParameterId, ParameterConfigType)\
namespace visr { namespace ril { \
template<> \
struct ParameterToId< ParameterClassType > \
{\
public:\
  static const ParameterType id = ParameterId;\
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
} }

#endif // #ifndef VISR_LIBRIL_PARAMETER_TYPE_HPP_INCLUDED
