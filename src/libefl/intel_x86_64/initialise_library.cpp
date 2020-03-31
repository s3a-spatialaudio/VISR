/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "initialise_library.hpp"

#include "vector_functions.hpp"
#include "cpu_features.hpp"

#include "../reference/vector_functions.hpp"

#include <immintrin.h>

namespace visr
{
namespace efl
{
namespace intel_x86_64
{

bool initialiseLibrary( char const * processor /*= ""*/ )
{
  // Determine CPU features
  CpuFeatures features{};

  // TODO: Set specialised versions based on the processor features.
  // NOTE: Functions that are not yet implemented are commented out.
  
  VectorMultiplyWrapper< float >::set( &intel_x86_64::vectorMultiply<float> );
  VectorMultiplyWrapper< double >::set( &intel_x86_64::vectorMultiply<double> );
  VectorMultiplyWrapper< std::complex<float> >::set( &intel_x86_64::vectorMultiply<std::complex<float> > );
  // VectorMultiplyInplaceWrapper< float >::set( &intel_x86_64::vectorMultiplyInplace<float> );
  // VectorMultiplyInplaceWrapper< std::complex<float> >::set( &intel_x86_64::vectorMultiplyInplace<std::complex<float> > );
  // VectorMultiplyConstantWrapper< float >::set( &intel_x86_64::vectorMultiplyConstant<float> );
  // VectorMultiplyConstantWrapper< std::complex<float> >::set( &intel_x86_64::vectorMultiplyConstant<std::complex<float> > );
  // VectorMultiplyConstantInplaceWrapper< float >::set( &intel_x86_64::vectorMultiplyConstantInplace<float> );
  // VectorMultiplyConstantInplaceWrapper< std::complex<float> >::set( &intel_x86_64::vectorMultiplyConstantInplace<std::complex<float> > );

  // VectorMultiplyAddWrapper< float >::set( &intel_x86_64::vectorMultiplyAdd<float> );
  // VectorMultiplyAddWrapper< double >::set( &intel_x86_64::vectorMultiplyAdd<double> );
  // VectorMultiplyAddWrapper< std::complex<float> >::set( &intel_x86_64::vectorMultiplyAdd<std::complex<float> > );
#if __FMA__ // The library has also to be compiled for this instruction set.
  if( features.hasFMA3() )
  {
    VectorMultiplyAddInplaceWrapper< float >::set( &intel_x86_64::vectorMultiplyAddInplace<float, Feature::FMA > );
    VectorMultiplyAddInplaceWrapper< double >::set( &intel_x86_64::vectorMultiplyAddInplace<double, Feature::FMA> );
    VectorMultiplyAddInplaceWrapper< std::complex<float> >::set( &intel_x86_64::vectorMultiplyAddInplace<std::complex<float>, Feature::FMA > );
  }
#endif // #if __FMA__

  // VectorMultiplyConstantAddWrapper< float >::set( &intel_x86_64::vectorMultiplyConstantAdd<float> );
  // VectorMultiplyConstantAddWrapper< std::complex<float> >::set( &intel_x86_64::vectorMultiplyConstantAdd<std::complex<float> > );
  VectorMultiplyConstantAddInplaceWrapper< float >::set( &intel_x86_64::vectorMultiplyConstantAddInplace<float> );
  VectorMultiplyConstantAddInplaceWrapper< std::complex<float> >::set( &intel_x86_64::vectorMultiplyConstantAddInplace<std::complex<float> > );

  return true;
}

bool uninitialiseLibrary()
{
  VectorMultiplyWrapper< float >::set( &reference::vectorMultiply<float> );
  VectorMultiplyWrapper< std::complex<float> >::set( &reference::vectorMultiply<std::complex<float> > );
  VectorMultiplyInplaceWrapper< float >::set( &reference::vectorMultiplyInplace<float> );
  VectorMultiplyInplaceWrapper< std::complex<float> >::set( &reference::vectorMultiplyInplace<std::complex<float> > );
  VectorMultiplyConstantWrapper< float >::set( &reference::vectorMultiplyConstant<float> );
  VectorMultiplyConstantWrapper< std::complex<float> >::set( &reference::vectorMultiplyConstant<std::complex<float> > );
  VectorMultiplyConstantInplaceWrapper< float >::set( &reference::vectorMultiplyConstantInplace<float> );
  VectorMultiplyConstantInplaceWrapper< std::complex<float> >::set( &reference::vectorMultiplyConstantInplace<std::complex<float> > );

  VectorMultiplyAddWrapper< float >::set( &reference::vectorMultiplyAdd<float> );
  VectorMultiplyAddWrapper< std::complex<float> >::set( &reference::vectorMultiplyAdd<std::complex<float> > );
  VectorMultiplyAddInplaceWrapper< float >::set( &reference::vectorMultiplyAddInplace<float> );
  VectorMultiplyAddInplaceWrapper< std::complex<float> >::set( &reference::vectorMultiplyAddInplace<std::complex<float> > );
  VectorMultiplyConstantAddWrapper< float >::set( &reference::vectorMultiplyConstantAdd<float> );
  VectorMultiplyConstantAddWrapper< std::complex<float> >::set( &reference::vectorMultiplyConstantAdd<std::complex<float> > );
  VectorMultiplyConstantAddInplaceWrapper< float >::set( &reference::vectorMultiplyConstantAddInplace<float> );
  VectorMultiplyConstantAddInplaceWrapper< std::complex<float> >::set( &reference::vectorMultiplyConstantAddInplace<std::complex<float> > );
  
  return true;
}

} // namespace armv7l_neon_32bit
} // namespace efl
} // namespace visr

