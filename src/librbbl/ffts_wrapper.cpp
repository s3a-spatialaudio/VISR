/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "ffts_wrapper.hpp"

#include <libefl/aligned_array.hpp>

#include <ffts.h>

#include <ciso646>
#include <stdexcept>

namespace visr
{
namespace rbbl
{

template<>
class FftsWrapper<float>::Impl
{
public:
  Impl( std::size_t fftSize, std::size_t /*alignElements*/ )
  {
    mFwdPlan = ffts_init_1d_real( fftSize, -1 );
    if( not mFwdPlan )
    {
      throw std::invalid_argument( "FfftWrapper: Initialisation of forward FFT plan failed." );
    }
    mInvPlan = ffts_init_1d_real( fftSize, 1 );
    if( not mInvPlan )
    {
      throw std::invalid_argument( "FfftWrapper: Initialisation of inverse FFT plan failed." );
    }
  }

  ~Impl()
  {
    ffts_free( mFwdPlan );
    ffts_free( mInvPlan );
  }


  ffts_plan_t* mFwdPlan;
  ffts_plan_t* mInvPlan;
};

template<typename DataType>
FftsWrapper<DataType>::FftsWrapper( std::size_t fftSize, std::size_t alignmentElements )
 : mImpl( new FftsWrapper<DataType>::Impl( fftSize, alignmentElements ) )
{
}
template FftsWrapper<float>::FftsWrapper( std::size_t, std::size_t );

template<typename DataType>
FftsWrapper<DataType>::~FftsWrapper()
{
}
template FftsWrapper<float>::~FftsWrapper( );
// template FftsWrapper<double>::~FftsWrapper( );

template<>
efl::ErrorCode FftsWrapper<float>::forwardTransform( float const * const in, std::complex<float> * out ) const
{
  ffts_execute( mImpl->mFwdPlan, in, out );
  return efl::noError; // apparently, there is no error reporting
}

template<>
efl::ErrorCode FftsWrapper<float>::inverseTransform( std::complex<float> const * const in, float * out ) const
{
  ffts_execute( mImpl->mInvPlan, in, out );
  return efl::noError; // apparently, there is no error reporting apart from a SIGSEGV in case of misalignment
}

#if 0
template<>
void FftsWrapper<double>::forwardTransform( double const * const in, std::complex<double> * out )
{
  fftw_execute_dft_r2c( mImpl->mFwdPlan, const_cast<double*>(in), reinterpret_cast<Impl::TransformDataType*>(out) );
}

template<>
void FftsWrapper<double>::inverseTransform( std::complex<double> const * const in, double * out )
{
  fftw_execute_dft_c2r( mImpl->mInvPlan, reinterpret_cast<Impl::TransformDataType*>(const_cast<std::complex<double>*>( in )), out );
}
#endif

} // namespace rbbl
} // namespace visr
