
/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_KISSFFT_WRAPPER_HPP_INCLUDED
#define VISR_LIBRBBL_KISSFFT_WRAPPER_HPP_INCLUDED

#include "fft_wrapper_base.hpp"

#include <memory>

namespace visr
{
namespace rbbl
{

/**
 * FFT wrapper class encapsulating the KissFFT library for real-to-complex transforms.
 * @tparam DataType The floating-point element type for the transform. The class is specialized for type \p float and \p double.
 */
template< typename DataType >
class KissFftWrapper: public FftWrapperBase<DataType>
{
public:
  /**
   * Typedef for the frequency-domain samples.
   * Needs to be redeclared and marked as 'typename' by GCC.
   */
  using FrequencyDomainType = typename FftWrapperBase<DataType>::FrequencyDomainType;

  KissFftWrapper( std::size_t fftSize, std::size_t alignment );

  ~KissFftWrapper();

  /*virtual*/ void forwardTransform( DataType const * const in, FrequencyDomainType * out );

  /*virtual*/ void inverseTransform( FrequencyDomainType const * const in, DataType * out );

  /*virtual*/ DataType forwardScalingFactor( ) const { return static_cast<DataType>(1.0); };

  /*virtual*/ DataType inverseScalingFactor() const { return static_cast<DataType>(1.0); }

private:
  /**
   * Internal implementation object to avoid Kiss dependencies in the header.
   * Holds the KissFFT configuration data for the forward and inverse transforms.
   */
  class Impl;
  /**
   * Pointer to the implementation object (pimpl idiom).
   */
  std::unique_ptr<Impl> mImpl;
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_KISSFFT_WRAPPER_HPP_INCLUDED
