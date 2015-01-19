/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBEFL_DENORMALISED_NUMBER_HANDLING_HPP_INCLUDED
#define VISR_LIBEFL_DENORMALISED_NUMBER_HANDLING_HPP_INCLUDED

#include "denormalised_number_handling.hpp"

#if(WIN32)
#include <xmmintrin.h>
#else // X86/X86_64 Unix architectures platforms
#include <xmmintrin.h>
#endif

#include <cstdint>
#include <ciso646>

namespace visr
{
namespace efl
{

class DenormalisedNumbers
{
  struct State
  {
    State()
     : flushToZero( false ), denormalsAreZero( false )
    {
    }

    explicit State( bool pFlushToZero, bool pDenormsAreZero )
     : flushToZero( pFlushToZero ), denormalsAreZero( pDenormsAreZero )
    {
    }

    bool flushToZero;
    bool denormalsAreZero;
  };

  State setDenormHandling( );

  void resetDenormHandling( State stateToRestore );

  /**
  */
  State setDenormHandling( State newState );
};

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_DENORMALISED_NUMBER_HANDLING_HPP_INCLUDED