/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "fftw_wrapper.hpp"

#include <libefl/aligned_array.hpp>

#include <fftw3.h>

#include <ciso646>
#include <stdexcept>

namespace visr
{
namespace rbbl
{

template<>
class FftwWrapper<float>::Impl
{
public:
  using OrigDataType = float;
  using TransformDataType = fftwf_complex;

  Impl( std::size_t fftSize, std::size_t alignElements )
  {
    efl::AlignedArray<float> in( fftSize, alignElements );
    std::size_t outputLength = 2 * (fftSize / 2 + fftSize % 2 + 1); // equivalent to ceil( fftSize/2+1 ), but without floating-point conversion.
    efl::AlignedArray<float> out( outputLength, alignElements );

    mFwdPlan = fftwf_plan_dft_r2c_1d( static_cast<int>(fftSize), in.data(), reinterpret_cast<TransformDataType*>(out.data()), FFTW_PRESERVE_INPUT );
    if( !mFwdPlan )
    {
      throw std::invalid_argument( "Initialisation of forward transform plan failed." );
    }
    mInvPlan = fftwf_plan_dft_c2r_1d( static_cast<int>(fftSize), reinterpret_cast<TransformDataType*>(out.data()), in.data(), FFTW_PRESERVE_INPUT );
    if( !mInvPlan )
    {
      throw std::invalid_argument( "Initialisation of inverse transform plan failed." );
    }
  }

  ~Impl()
  {
    fftwf_destroy_plan( mFwdPlan );
    fftwf_destroy_plan( mInvPlan );
  }

  fftwf_plan mFwdPlan;
  fftwf_plan mInvPlan;
};

template<>
class FftwWrapper<double>::Impl
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

template<typename DataType>
FftwWrapper<DataType>::FftwWrapper( std::size_t fftSize, std::size_t alignmentElements )
 : mImpl( new FftwWrapper<DataType>::Impl( fftSize, alignmentElements ) )
{
}
template FftwWrapper<float>::FftwWrapper( std::size_t, std::size_t );
template FftwWrapper<double>::FftwWrapper( std::size_t, std::size_t );

template<typename DataType>
FftwWrapper<DataType>::~FftwWrapper()
{
}
template FftwWrapper<float>::~FftwWrapper( );
template FftwWrapper<double>::~FftwWrapper( );

template<>
efl::ErrorCode FftwWrapper<float>::forwardTransform( float const * const in, std::complex<float> * out )
{
  fftwf_execute_dft_r2c( mImpl->mFwdPlan, const_cast<float*>(in), reinterpret_cast<Impl::TransformDataType*>(out) );
  return efl::noError; // apparently, there is no error reporting apart from a SIGSEGV in case of misalignment
}

template<>
efl::ErrorCode FftwWrapper<float>::inverseTransform( std::complex<float> const * const in, float * out )
{
  fftwf_execute_dft_c2r( mImpl->mInvPlan, reinterpret_cast<Impl::TransformDataType*>(const_cast<std::complex<float>* >(in)), out );
  return efl::noError; // apparently, there is no error reporting apart from a SIGSEGV in case of misalignment
}

template<>
efl::ErrorCode FftwWrapper<double>::forwardTransform( double const * const in, std::complex<double> * out )
{
  fftw_execute_dft_r2c( mImpl->mFwdPlan, const_cast<double*>(in), reinterpret_cast<Impl::TransformDataType*>(out) );
  return efl::noError; // apparently, there is no error reporting apart from a SIGSEGV in case of misalignment
}

template<>
efl::ErrorCode FftwWrapper<double>::inverseTransform( std::complex<double> const * const in, double * out )
{
  fftw_execute_dft_c2r( mImpl->mInvPlan, reinterpret_cast<Impl::TransformDataType*>(const_cast<std::complex<double>*>( in )), out );
  return efl::noError; // apparently, there is no error reporting apart from a SIGSEGV in case of misalignment
}

} // namespace rbbl
} // namespace visr
