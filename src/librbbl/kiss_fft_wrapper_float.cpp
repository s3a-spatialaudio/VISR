/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "kiss_fft_wrapper.hpp"

#include <kiss_fftr_float.h>

#include <ciso646>
#include <stdexcept>

namespace visr
{
namespace rbbl
{

template<>
class KissFftWrapper<float>::Impl
{
public:
  using OrigDataType = float;
  using TransformDataType = kiss_fft_cpx;
  
  Impl( std::size_t fftSize, std::size_t alignElements )
  {
    mFwdPlan = kiss_fftr_alloc_float( static_cast<int>(fftSize), 0/*forward FFT*/, 0, 0 );
    if( !mFwdPlan )
    {
      throw std::invalid_argument( "Initialisation of forward transform plan failed." );
    }
    mInvPlan = kiss_fftr_alloc_float( static_cast<int>(fftSize), 1/*inverse FFT*/, 0, 0 );
    if( !mInvPlan )
    {
      throw std::invalid_argument( "Initialisation of inverse transform plan failed." );
    }
  }

  ~Impl()
  {
    kiss_fftr_free( mFwdPlan );
    kiss_fftr_free( mInvPlan );
  }

  kiss_fftr_cfg_float mFwdPlan;
  kiss_fftr_cfg_float mInvPlan;
};

template<>
KissFftWrapper<float>::KissFftWrapper( std::size_t fftSize, std::size_t alignmentElements )
 : mImpl( new KissFftWrapper<float>::Impl( fftSize, alignmentElements ) )
{
}

template<>
KissFftWrapper<float>::~KissFftWrapper()
{
}

template<>
efl::ErrorCode KissFftWrapper<float>::forwardTransform( float const * const in, std::complex<float> * out ) const
{
  kiss_fftr_float( mImpl->mFwdPlan, const_cast<float*>(in), reinterpret_cast<Impl::TransformDataType*>(out) );
  return efl::noError; // apparently, there is no error reporting.
}

template<>
efl::ErrorCode KissFftWrapper<float>::inverseTransform( std::complex<float> const * const in, float * out ) const
{
  kiss_fftri_float( mImpl->mInvPlan, reinterpret_cast<Impl::TransformDataType*>(const_cast<std::complex<float>* >(in)), out );
  return efl::noError; // apparently, there is no error reporting.
}

} // namespace rbbl
} // namespace visr
