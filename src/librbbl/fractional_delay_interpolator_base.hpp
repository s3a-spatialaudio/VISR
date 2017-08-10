/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_FRACTIONAL_DELAY_BASE_HPP_INCLUDED
#define VISR_LIBRBBL_FRACTIONAL_DELAY_BASE_HPP_INCLUDED

#include "export_symbols.hpp"

#include <memory>

namespace visr
{
namespace rbbl
{

/**
 * Abstract base class for interpolation algorithms on discrete-time sequences.
 */
template <typename SampleType>
class VISR_RBBL_LIBRARY_SYMBOL FractionalDelayBase
{
public:
  virtual ~FractionalDelayBase();

  virtual void interpolate( SampleType const * basePointer,
                            SampleType * result
                            SampleType startDelay, SampleType endDelay,
                            SampleType startGain, SampleType endGain ) = 0;
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_FRACTIONAL_DELAY_BASE_HPP_INCLUDED
