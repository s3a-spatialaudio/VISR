/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "denormalised_number_handling.hpp"

namespace visr
{
namespace efl
{

// only for Intel X86/X86_64 platforms
namespace // unnamed
{
  using ControlWord = uint32_t;

  static const ControlWord cDenormsAreZeroMask = 1 << 6;

  static const ControlWord cFlushToZeroMask = 1 << 15;

  static const ControlWord cCwMask = ~(cDenormsAreZeroMask bitor cFlushToZeroMask);

} // unnamed namespace

DenormalisedNumbers::State DenormalisedNumbers::setDenormHandling( )
{
  return setDenormHandling( State( true, true ) );
}

void DenormalisedNumbers::resetDenormHandling( State stateToRestore )
{
  setDenormHandling( stateToRestore );
}

DenormalisedNumbers::State DenormalisedNumbers::setDenormHandling( State newState )
{
  State oldState;

  ControlWord const oldCW = _mm_getcsr( );

  bool const oldDAZ = (oldCW bitand cDenormsAreZeroMask);
  bool const oldFTZ = (oldCW bitand cFlushToZeroMask);

  ControlWord newCW = oldCW bitand cCwMask;
  if( newState.denormalsAreZero )
  {
    newCW = newCW bitor cDenormsAreZeroMask;
  }
  if( newState.flushToZero )
  {
    newCW = newCW bitor cFlushToZeroMask;
  }
  _mm_setcsr( newCW );

  return State( oldDAZ, oldFTZ );
}

} // namespace efl
} // namespace visr
