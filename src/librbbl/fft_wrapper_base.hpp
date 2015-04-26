/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_FFT_WRAPPER_BASE_HPP_INCLUDED
#define VISR_LIBRBBL_FFT_WRAPPER_BASE_HPP_INCLUDED

#include <cstddef>
#include <complex>

namespace visr
{
namespace rbbl
{

template< typename DataType >
class FftWrapperBase
{
public:
  using FrequencyDomainType = std::complex<DataType>;

  virtual ~FftWrapperBase() {};

  virtual void init( std::size_t fftSize, std::size_t alignment ) = 0;

  virtual void forwardTransform( DataType const * const in, FrequencyDomainType * out ) = 0;

  virtual void inverseTransform( FrequencyDomainType const * const in, DataType * out ) = 0;
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_FFT_WRAPPER_BASE_HPP_INCLUDED
