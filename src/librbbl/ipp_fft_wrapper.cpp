/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "ipp_fft_wrapper.hpp"

#include <libefl/aligned_array.hpp>

#include <ipps.h>

#include <cassert>
#include <ciso646>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <tuple>

namespace visr
{
namespace rbbl
{

namespace
{

std::pair< bool, std::size_t > getFftOrder( std::size_t fftSize )
{
  std::size_t logVal = static_cast<std::size_t>(std::round(std::log2( static_cast<double>(fftSize ))));
  if( std::pow( 2, logVal ) != fftSize )
  {
    return std::make_pair( false, 0 );
  }
  else
  {
    return std::make_pair( true, logVal );
  }
}

}

  template<>
  class IppFftWrapper<float>::Impl
  {
  public:
    using OrigDataType = float;
    using TransformDataType = Ipp32f; // should be a typedef for 'float'

    Impl( std::size_t fftSize, std::size_t alignElements )
      : mWorkBuffer( sizeof(OrigDataType)* alignElements )
      , mSpecBuffer( sizeof(OrigDataType)* alignElements )
    {
      bool isPowerOfTwo = false;
      std::size_t fftOrder;
      std::tie( isPowerOfTwo, fftOrder) = getFftOrder( fftSize );
      if( !isPowerOfTwo )
      {
        throw std::invalid_argument( "IppFftWrapper: FFT order is not an integer power of 2." );
      }

      int specSize = 0, specBufferSize = 0, workBufferSize = 0;
      IppStatus sts;
      if( (sts = ippsFFTGetSize_R_32f( static_cast<int>(fftOrder), IPP_FFT_DIV_BY_SQRTN, ippAlgHintNone,
        &specSize, &specBufferSize, &workBufferSize )) != ippStsNoErr )
      {
        throw std::invalid_argument( "IppFftWrapper: Determination of IPP FFT structure sizes failed." );
      }
      mSpecBuffer.resize( static_cast<std::size_t>(specSize) );
      mWorkBuffer.resize( static_cast<std::size_t>(workBufferSize) );

      // Buffer used during the FFT_init function
      efl::AlignedArray<Ipp8u> initSpecBuffer( static_cast<std::size_t>(specBufferSize), sizeof(OrigDataType)*alignElements );

      if( (sts = ippsFFTInit_R_32f( &mFftSpec, static_cast<int>(fftOrder), IPP_FFT_DIV_BY_SQRTN, ippAlgHintNone, mSpecBuffer.data( ), initSpecBuffer.data( ) )) != ippStsNoErr )
      {
        throw std::invalid_argument( "IppFftWrapper: Initialisation of FFT spec structure failed." );
      }
    }

    ~Impl( )
    {
      // nothing to be done here, memory is deallocated automatically.
    }

    /**
    * Memory to be used by the transform functions.
    */
    efl::AlignedArray<Ipp8u> mWorkBuffer;

    /**
    * Memory buffer in which the FFT sspecification structure is allocated.
    */
    efl::AlignedArray<Ipp8u> mSpecBuffer;

    /**
    * Pointer to the internal FFT specification structure.
    * According to the IIPP docu
    */
    IppsFFTSpec_R_32f* mFftSpec;
  };
  
template<>
class IppFftWrapper<double>::Impl
{
public:
  using OrigDataType = double;
  using TransformDataType = Ipp64f; // should be a typedef for 'double'

  Impl( std::size_t fftSize, std::size_t alignElements )
    : mWorkBuffer( sizeof(OrigDataType) * alignElements )
    , mSpecBuffer( sizeof(OrigDataType)* alignElements )
  {
    bool isPowerOfTwo = false;
    std::size_t fftOrder;
    std::tie( isPowerOfTwo, fftOrder ) = getFftOrder( fftSize );
    if( !isPowerOfTwo )
    {
      throw std::invalid_argument( "IppFftWrapper: FFT order is not an integer power of 2." );
    }

    int specSize = 0, specBufferSize = 0, workBufferSize = 0;
    IppStatus sts;
    if( (sts = ippsFFTGetSize_R_64f( static_cast<int>(fftOrder), IPP_FFT_DIV_BY_SQRTN, ippAlgHintNone,
          &specSize, &specBufferSize, &workBufferSize )) != ippStsNoErr )
    {
      throw std::invalid_argument( "IppFftWrapper: Determination of IPP FFT structure sizes failed." );
    }
    mSpecBuffer.resize( static_cast<std::size_t>(specSize) );
    mWorkBuffer.resize( static_cast<std::size_t>(workBufferSize) );

    // Buffer used during the FFT_init function
    efl::AlignedArray<Ipp8u> initSpecBuffer( static_cast<std::size_t>(specBufferSize), sizeof(OrigDataType)*alignElements );

    if( (sts = ippsFFTInit_R_64f( &mFftSpec, static_cast<int>(fftOrder), IPP_FFT_DIV_BY_SQRTN, ippAlgHintNone, mSpecBuffer.data( ), initSpecBuffer.data() )) != ippStsNoErr )
    {
      throw std::invalid_argument( "IppFftWrapper: Initialisation of FFT spec structure failed." );
    }
  }

  ~Impl()
  {
    // nothing to be done here, memory is deallocated automatically.
  }

  /**
   * Memory to be used by the transform functions.
   */
  efl::AlignedArray<Ipp8u> mWorkBuffer;

  /**
   * Memory buffer in which the FFT sspecification structure is allocated.
   */
  efl::AlignedArray<Ipp8u> mSpecBuffer;

  /**
   * Pointer to the internal FFT specification structure.
   * According to the IIPP docu
   */
  IppsFFTSpec_R_64f* mFftSpec;
};

template<typename DataType>
IppFftWrapper<DataType>::IppFftWrapper( std::size_t fftSize, std::size_t alignmentElements )
 : mImpl( new IppFftWrapper<DataType>::Impl( fftSize, alignmentElements ) )
{
}
template IppFftWrapper<float>::IppFftWrapper( std::size_t, std::size_t );
template IppFftWrapper<double>::IppFftWrapper( std::size_t, std::size_t );

template<typename DataType>
IppFftWrapper<DataType>::~IppFftWrapper()
{
}
template IppFftWrapper<float>::~IppFftWrapper( );
template IppFftWrapper<double>::~IppFftWrapper( );

template<>
efl::ErrorCode IppFftWrapper<float>::forwardTransform( float const * const in, std::complex<float> * out )
{
  IppStatus const sts = ippsFFTFwd_RToCCS_32f( in, reinterpret_cast<Impl::TransformDataType*>(out), mImpl->mFftSpec, mImpl->mWorkBuffer.data( ) );
  return sts == ippStsNoErr ? efl::noError : efl::arithmeticError;
}

template<>
efl::ErrorCode IppFftWrapper<float>::inverseTransform( std::complex<float> const * const in, float * out )
{
  IppStatus const sts = ippsFFTInv_CCSToR_32f( reinterpret_cast<const Impl::TransformDataType*>(in), out, mImpl->mFftSpec, mImpl->mWorkBuffer.data( ) );
  return sts == ippStsNoErr ? efl::noError : efl::arithmeticError;
}

template<>
efl::ErrorCode IppFftWrapper<double>::forwardTransform( double const * const in, std::complex<double> * out )
{
  IppStatus const sts = ippsFFTFwd_RToCCS_64f( in, reinterpret_cast<Impl::TransformDataType*>(out), mImpl->mFftSpec, mImpl->mWorkBuffer.data( ) );
  return sts == ippStsNoErr ? efl::noError : efl::arithmeticError;
}

template<>
efl::ErrorCode IppFftWrapper<double>::inverseTransform( std::complex<double> const * const in, double * out )
{
  IppStatus const sts = ippsFFTInv_CCSToR_64f( reinterpret_cast<const Impl::TransformDataType*>(in), out, mImpl->mFftSpec, mImpl->mWorkBuffer.data( ) );
  return sts == ippStsNoErr ? efl::noError : efl::arithmeticError;
}


} // namespace rbbl
} // namespace visr
