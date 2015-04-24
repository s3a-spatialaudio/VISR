/* Copyright Institute of Sound and Vibration Research - All rights reserved */
#ifndef VISR_LIBRBBL_FFTW_WRAPPER_HPP_INCLUDED
#define VISR_LIBRBBL_FFTW_WRAPPER_HPP_INCLUDED

#include "fft_wrapper_base.hpp"

#include <memory>

namespace visr
{
namespace rbbl
{

template< typename DataType >
class FftwWrapper: public FftWrapperBase<DataType>
{
public:
  FftwWrapper();

  ~FftwWrapper();

  /*virtual*/ void init( std::size_t fftSize, std::size_t alignment );

  /*virtual*/ void forwardFft( DataType const * const in, DataType * out );

  /*virtual*/ void inverseFft( DataType const * const in, DataType * out );
private:
  /**
   * Internal implementation object to avoid FFTW dependencies in the header.
   * Holds the FFTW plans and any other required data structures.
   */
  class Impl;
  /**
   * Pointer to the implementation object (pimpl idiom).
   */
  std::unique_ptr<Impl> mImpl;
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_FFTW_WRAPPER_HPP_INCLUDED
