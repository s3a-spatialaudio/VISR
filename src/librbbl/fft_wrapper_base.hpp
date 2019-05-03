/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_FFT_WRAPPER_BASE_HPP_INCLUDED
#define VISR_LIBRBBL_FFT_WRAPPER_BASE_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libefl/error_codes.hpp>

#include <cstddef>
#include <complex>

namespace visr
{
namespace rbbl
{

template< typename DataType >
class VISR_RBBL_LIBRARY_SYMBOL FftWrapperBase
{
public:
  using FrequencyDomainType = std::complex<DataType>;

  virtual ~FftWrapperBase() {};

  virtual efl::ErrorCode forwardTransform( DataType const * const in, FrequencyDomainType * out ) const = 0;

  virtual efl::ErrorCode inverseTransform( FrequencyDomainType const * const in, DataType * out ) const = 0;

  virtual DataType forwardScalingFactor() const = 0;

  virtual DataType inverseScalingFactor( ) const = 0;
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_FFT_WRAPPER_BASE_HPP_INCLUDED
