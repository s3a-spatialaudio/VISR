/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_GAIN_MATRIX_HPP_INCLUDED
#define VISR_LIBRBBL_GAIN_MATRIX_HPP_INCLUDED

#include "export_symbols.hpp"
#include "gain_fader.hpp"

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
class VISR_RBBL_LIBRARY_SYMBOL GainMatrix
{
public:
  /**
   * Constructor, sets all matrix gains to common value (default: 0.0)
  * @param numberOfInputs The number of input channels of the audio processor, i.e., the number of columns of the matrix.
  * @param numberOfOutputs The number of output channels of the audio processor, i.e., the number of rows of the matrix.
  * @param blockLength The number of samples in each input or output channel processed in each invocation of process()
  * @param interpolationSteps The duration of a transition to a new set of gain value, specified in samples. Must be
  * an integer multiple of the \p blockLength parameter.
  * @param initialValue The initial value (linear scale) for all gain values in the matrix.
  * @param alignment the minimum alignment for the input and output vectors as well as the internally stored data members,
  * given as a  number of samples.
   */
  explicit GainMatrix( std::size_t numberOfInputs,
                       std::size_t numberOfOutputs,
                       std::size_t blockLength,
                       std::size_t interpolationSteps,
                       ElementType initialValue = static_cast<ElementType>(0.0),
                       std::size_t alignment = 0 );

  /**
  * Constructor, sets matrix gains according to a given matrix.
  * @param numberOfInputs The number of input channels of the audio processor, i.e., the number of columns of the matrix.
  * @param numberOfOutputs The number of output channels of the audio processor, i.e., the number of rows of the matrix.
  * @param blockLength The number of samples in each input or output channel processed in each invocation of process()
  * @param interpolationSteps The duration of a transition to a new set of gain value, specified in samples. Must be
  * an integer multiple of the \p blockLength parameter.
  * @param initialGains The initial matrix gains (in linear scale). Must be a matrix with dimension \p numberOfOutputs * \p numberOfInputs .
  * @param alignment The minimum alignment for the input and output vectors as well as the internally stored data members,
  * given as a  number of samples.
  */
  explicit GainMatrix( std::size_t numberOfInputs,
                       std::size_t numberOfOutputs,
                       std::size_t blockLength,
                       std::size_t interpolationSteps,
                       efl::BasicMatrix<ElementType> const& initialGains,
                       std::size_t alignment = 0 );

  /**
   * Destructor.
   */
  ~GainMatrix();

  /**
   * Process multichannel audio by matrixing the number of \p blockLength samples (specified in the constructor) from each
   * input channel to the range of the output channels. If a new set of gain values has been set previously, the
   * transition to the new value will continue. After that, matrixing will be performed using the new gain values.
   * @param input A range of arrays containing the input sample vectors. Must be \p numberOfInputs elements long, and each
   * sample vector must contain at least \p blockLength elements.
   * @param[out] output Range of arrays containing the output sample vectors. Must be \p numberOfOutputs elements long, and each
   * sample vector must contain at least \p blockLength elements.
   */
  void process( ElementType const * const * input, ElementType * const * output );

  /**
   * Set a new matrix of gain values and process multichannel audio by matrixing the number of \p blockLength samples
   * (specified in the constructor) from each input channel to the range of the output channels.
   * If no new set of gain values has been set previously, a transition will start that will change the used gains to the new
   * values over a period of \p interpolationSteps samples. If there is a transition currently active, the current interpolated
   * gains will be used as the starting point of a new transistion process (taking \p interpolationSteps samples).
   * transition to the new value will continue. After that, matrixing will be performed using the new gain values.
   * @param input A range of arrays containing the input sample vectors. Must be \p numberOfInputs elements long, and each
   * sample vector must contain at least \p blockLength elements.
   * @param[out] output The output Range of arrays containing the output sample vectors. Must be \p numberOfOutputs elements long, and each
   * sample vector must contain at least \p blockLength elements.
   * @param newGains The matrix of new gain values, dimension must be \p numerOfInputs * \p numberOfOutputs .
   */
  void process( ElementType const * const * input, ElementType * const * output,
                efl::BasicMatrix<ElementType> const& newGains);

  /**
   * Set a new gain matrix.
   * If no new set of gain values has been set previously, a transition will start that will change the used gains to the new
   * values over a period of \p interpolationSteps samples. If there is a transition currently active, the current interpolated
   * gains will be used as the starting point of a new transistion process (taking \p interpolationSteps samples).
   * @param newGains The matrix of new gain values, dimension must be \p numerOfInputs * \p numberOfOutputs .
   */
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
   * @param newGains The new gain matrix to be set.
   */
  void setGainsInternal( efl::BasicMatrix<ElementType> const & newGains );

  /**
   * The gains used as the starting point of the most recent transition
   * (even if it has already finished).
   */
  efl::BasicMatrix< ElementType > mPreviousGains;

  /**
   * The gains used as destination values of the current transition.
   */
  efl::BasicMatrix< ElementType > mNextGains;

  /**
   * The number of samples processed in each invocation of process.
   */
  std::size_t const mBlockSize;

  /**
   * Guarantueed alignment for input and output blocks (in number of elements.
   * The same alignment value is also used to set up the internal data strucutures.
   */
  std::size_t const mAlignment;

  /**
   * Counter denoting the current position within the transition process.
   * The final value \p mInterpolationPeriods denotes the previous iteration has terminated.
   */
  std::size_t mInterpolationCounter;

  /**
   * Object that implements the 'ramping' of gain values.
   */
  GainFader< ElementType > mFader;
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_GAIN_MATRIX_HPP_INCLUDED
