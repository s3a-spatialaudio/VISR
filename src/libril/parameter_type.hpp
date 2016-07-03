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

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_PARAMETER_TYPE_HPP_INCLUDED
