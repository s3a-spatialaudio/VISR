
/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_FFTS_WRAPPER_HPP_INCLUDED
#define VISR_LIBRBBL_FFTS_WRAPPER_HPP_INCLUDED

#include "fft_wrapper_base.hpp"

#include <memory>

namespace visr
{
namespace rbbl
{

/**
 * FFT wrapper class encapsulating the FFTW library for real-to-complex transforms.
 * @tparam DataType The floating-point element type for the transform. The class is specialized for type \p float and \p double.
 */
template< typename DataType >
class FftsWrapper: public FftWrapperBase<DataType>
{
public:
  /**
   * Typedef for the frequency-domain samples.
   * Needs to be redeclared and marked as 'typename' by GCC.
   */
  using FrequencyDomainType = typename FftWrapperBase<DataType>::FrequencyDomainType;

  FftsWrapper( std::size_t fftSize, std::size_t alignment );

  ~FftsWrapper();

  /*virtual*/ efl::ErrorCode forwardTransform( DataType const * const in, FrequencyDomainType * out ) const;

  /*virtual*/ efl::ErrorCode inverseTransform( FrequencyDomainType const * const in, DataType * out ) const;

  /*virtual*/ DataType forwardScalingFactor( ) const { return static_cast<DataType>(1.0); };

  /*virtual*/ DataType inverseScalingFactor() const { return static_cast<DataType>(1.0); }

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
  
#endif // #ifndef VISR_LIBRBBL_FFTS_WRAPPER_HPP_INCLUDED
