/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "kiss_fft_wrapper.hpp"

#include <kiss_fftr_double.h>

#include <ciso646>
#include <stdexcept>

namespace visr
{
namespace rbbl
{

template<>
class KissFftWrapper<double>::Impl
{
public:
  using OrigDataType = double;
  using TransformDataType = kiss_fft_cpx;
  
  Impl( std::size_t fftSize, std::size_t alignElements )
  {
    mFwdPlan = kiss_fftr_alloc_double( static_cast<int>(fftSize), 0/*forward FFT*/, 0, 0 );
    if( !mFwdPlan )
    {
      throw std::invalid_argument( "Initialisation of forward transform plan failed." );
    }
    mInvPlan = kiss_fftr_alloc_double( static_cast<int>(fftSize), 1/*inverse FFT*/, 0, 0 );
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

  kiss_fftr_cfg_double mFwdPlan;
  kiss_fftr_cfg_double mInvPlan;
};

template<>
KissFftWrapper<double>::KissFftWrapper( std::size_t fftSize, std::size_t alignmentElements )
 : mImpl( new KissFftWrapper<double>::Impl( fftSize, alignmentElements ) )
{
}

template<>
KissFftWrapper<double>::~KissFftWrapper()
{
}

template<>
void KissFftWrapper<double>::forwardTransform( double const * const in, std::complex<double> * out )
{
  kiss_fftr_double( mImpl->mFwdPlan, const_cast<double*>(in), reinterpret_cast<Impl::TransformDataType*>(out) );
}

template<>
void KissFftWrapper<double>::inverseTransform( std::complex<double> const * const in, double * out )
{
  kiss_fftri_double( mImpl->mInvPlan, reinterpret_cast<Impl::TransformDataType*>(const_cast<std::complex<double>* >(in)), out );
}

} // namespace rbbl
} // namespace visr
