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

#if 0
template<>
class FftsWrapper<double>::Impl
{
public:
  using OrigDataType = double;
  using TransformDataType = fftw_complex;

  Impl( std::size_t fftSize, std::size_t alignElements )
  {
    efl::AlignedArray<double> in( fftSize, alignElements );
    std::size_t outputLength = 2 * (fftSize / 2 + fftSize % 2 + 1); // equivalent to ceil( fftSize/2+1 ), but without floating-point conversion.
    efl::AlignedArray<double> out( outputLength, alignElements );

    mFwdPlan = fftw_plan_dft_r2c_1d( static_cast<int>(fftSize), in.data(), reinterpret_cast<TransformDataType*>(out.data()), FFTW_PRESERVE_INPUT );
    if( !mFwdPlan )
    {
      throw std::invalid_argument( "Initialisation of forward transform plan failed." );
    }
    mInvPlan = fftw_plan_dft_c2r_1d( static_cast<int>(fftSize), reinterpret_cast<TransformDataType*>(out.data()), in.data(), FFTW_PRESERVE_INPUT );
    if( !mInvPlan )
    {
      throw std::invalid_argument( "Initialisation of inverse transform plan failed." );
    }
  }

  ~Impl()
  {
    fftw_destroy_plan( mFwdPlan );
    fftw_destroy_plan( mInvPlan );
  }

  fftw_plan mFwdPlan;
  fftw_plan mInvPlan;
};
#endif

template<typename DataType>
FftsWrapper<DataType>::FftsWrapper( std::size_t fftSize, std::size_t alignmentElements )
 : mImpl( new FftsWrapper<DataType>::Impl( fftSize, alignmentElements ) )
{
}
template FftsWrapper<float>::FftsWrapper( std::size_t, std::size_t );
// template FftsWrapper<double>::FftsWrapper( std::size_t, std::size_t );

template<typename DataType>
FftsWrapper<DataType>::~FftsWrapper()
{
}
template FftsWrapper<float>::~FftsWrapper( );
// template FftsWrapper<double>::~FftsWrapper( );

template<>
void FftsWrapper<float>::forwardTransform( float const * const in, std::complex<float> * out )
{
  ffts_execute( mImpl->mFwdPlan, in, out );
}

template<>
void FftsWrapper<float>::inverseTransform( std::complex<float> const * const in, float * out )
{
  ffts_execute( mImpl->mInvPlan, in, out );
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
