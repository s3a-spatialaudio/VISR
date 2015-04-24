/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_FFT_WRAPPER_BASE_HPP_INCLUDED
#define VISR_LIBRBBL_FFT_WRAPPER_BASE_HPP_INCLUDED

#include <cstddef>

namespace visr
{
namespace rbbl
{

template< typename DataType >
class FftWrapperBase
{
public:
  virtual ~FftWrapperBase() {};

  virtual void init( std::size_t fftSize, std::size_t alignment ) = 0;

  virtual void forwardFft( DataType const * const in, DataType * out ) = 0;

  virtual void inverseFft( DataType const * const in, DataType * out ) = 0;
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_FFT_WRAPPER_BASE_HPP_INCLUDED
