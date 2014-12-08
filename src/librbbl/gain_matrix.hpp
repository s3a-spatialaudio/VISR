/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_GAIN_MATRIX_HPP_INCLUDED
#define VISR_LIBRBBL_GAIN_MATRIX_HPP_INCLUDED

#include<libefl/aligned_array.hpp>
#include<libefl/basic_matrix.hpp>

namespace visr
{
namespace rbbl
{

/**
 * Processing component to apply a potentially time-varying gain 
 * matrix operation to a vector of audio input signals, producing a vector output signals
 * @tparam ElementType The data type used for the audio samples and the gain values. 
 * Class is explicitly instantiated for element types float and double.
 */
template< typename ElementType >
class GainMatrix
{
public:
  /**
   * Constructor, sets all matrix gains to common value (default: 0.0)
   * @param numberOfInputs
   * @param numberOfOutputs
   * @param blockLength
   * @param interpolationSteps
   * @param initialValue
   * @param alignment
   */
  explicit GainMatrix( std::size_t numberOfInputs,
                       std::size_t numberOfOutputs,
                       std::size_t blockLength,
                       std::size_t interpolationSteps,
                       ElementType initialValue = static_cast<ElementType>(0.0),
                       std::size_t alignment = 0 );

  /**
  * Constructor, sets matrix gains according to a given matrix.
  * @param numberOfInputs
  * @param numberOfOutputs
  * @param blockLength
  * @param interpolationSteps
  * @param initialValue
  * @param alignment
  */
  explicit GainMatrix( std::size_t numberOfInputs,
                       std::size_t numberOfOutputs,
                       std::size_t blockLength,
                       std::size_t interpolationSteps,
                       efl::BasicMatrix<ElementType> const& initialGains,
                       std::size_t alignment = 0 );

  ~GainMatrix();

  /**
   */
  void process( ElementType const * const * input, ElementType * const * output );

  void process( ElementType const * const * input, ElementType * const * output,
                efl::BasicMatrix<ElementType> const& newGains);

  void setNewGains( efl::BasicMatrix<ElementType> const& newGains );

private:
  /**
   * Internal implementation method for applying 
   * the matrix gains to the audio signals.
   * Calculating and advancing the interpolation ratio is not done here.
   * @see process()
   */
  void processAudio( ElementType const * const * input, ElementType * const * output );

  /**
   * Internal method for applying a new set of matrix gains.
   * @param The new gain matrix to be set.
   */
  void setGainsInternal( efl::BasicMatrix<ElementType> const & newGains );

  efl::BasicMatrix< ElementType > mPreviousGains;

  efl::BasicMatrix< ElementType > mNextGains;

  std::size_t const mBlockSize;

  std::size_t const mInterpolationPeriods;

  std::size_t mInterpolationCounter;

  efl::AlignedArray< ElementType > mRamp;

  efl::AlignedArray< ElementType > mTempBuffer;

};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_GAIN_MATRIX_HPP_INCLUDED
