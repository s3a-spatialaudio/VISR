/* Copyright Institute of Sound and Vibration Research - All rights reserved */

/**
* @file libefl/lagrange_interpolator.hpp
* @author Andreas Franck a.franck@soton.ac.uk
*/

#ifndef VISR_LIBEFL_LAGRANGE_INTERPOLATOR_HPP_INCLUDED
#define VISR_LIBEFL_LAGRANGE_INTERPOLATOR_HPP_INCLUDED

#include <array>

// #define LAGRANGE_INTERPOLATOR_GENERATE_STAGE_TABLES

namespace visr
{
namespace efl
{

/**
 * Class template to calculate the parameters of a Lagrange interpolator.
 * @param DataType The numerical type of the sequence samples to be interpolated.
 * @param N The order of Lagrange interpolation
 * @tparam reverseOrder Whether the output coefficients are stored in normal FIR filter order (false) or reversed (true).
 * The latter  enables filtering via a dot product without reversing one of the sequences.
 */
template< typename DataType, std::size_t N, bool reverseCoeffs = false >
class LagrangeInterpolator
{
public:
  /**
   * Default constructor, initialises the constant coefficients.
   */
  LagrangeInterpolator();

  /**
   * Calculation function.
   */
  void calculateCoefficients( DataType mu, std::array<DataType,N+1> & result ) const;


  static constexpr std::size_t storageSize() { return internalStorageSize(N+1); }
private:


  static constexpr std::size_t internalStorageSize( size_t length )
  {
    return length <= 2 ? length : length + internalStorageSize( length / 2 );
  }

  static constexpr std::size_t numberOfStages()
  {
    return numberOfStagesImpl( N+1 );
  }

  static constexpr std::size_t stageSize( std::size_t stage )
  {
    return stage == 0 ? N+1 : stageSize( stage - 1 ) / 2;
  }

  static constexpr std::size_t stageOffset( std::size_t stage )
  {
    return stage == 0 ? 0 : stageOffset( stage - 1 ) + stageSize( stage - 1 );
  }

  template<std::size_t length>
  void multiplyAndShuffle( DataType * coeffs ) const;
  //{
  //  static constexpr bool oddLength = (length % 2) != 0;
  //  std::array<DataType, length/2> prod;
  //  static std::size_t constexpr pairs = length / 2;
  //  for( std::size_t idx(0); idx < pairs; ++idx )
  //  {
  //    prod[idx] = coeffs[2*idx] * coeffs[2*idx]+1;
  //  }
  //  // If there is a single coefficient at the last position, leave it as it is.
  //  if( length % 2 != 0 )
  //  {
  //    prod[pairs] = coeffs[length-2];
  //  }
  //  multiplyAndShuffle<(length+1)/2>( &prod[0] );
  //}

  //template<>
  //void multiplyAndShuffle<2>(  DataType * coeffs )
  //{
  //  std::swap( coeffs[0], coeffs[1] );
  //}

//  template<>
//  void multiplyAndShuffle<1>(  DataType * coeffs )
//  {
//    // nothing to do
//  }

  static DataType offset( std::size_t idx );

  static DataType scaleFactor( std::size_t idx );

  static constexpr std::size_t numberOfStagesImpl( std::size_t length )
  {
    return length <= 1 ? 1 : 1 + numberOfStagesImpl( length / 2 );
  }

  static std::array<DataType, N+1> generateOffsets();

  static std::array<DataType, N+1> generateScaleFactors();

#if LAGRANGE_INTERPOLATOR_GENERATE_STAGE_TABLES
  static std::array<std::size_t, LagrangeInterpolator<DataType,N>::numberOfStages()> generateStageSizes();

  static std::array<std::size_t, LagrangeInterpolator<DataType,N>::numberOfStages()> generateStageOffsets();
#endif
  mutable std::array<DataType, internalStorageSize(N+1)> mInternalCoeffs;

  const std::array<DataType, N+1> cOffsets;

  const std::array<DataType, N+1> cScaleFactors;

#if LAGRANGE_INTERPOLATOR_GENERATE_STAGE_TABLES
  static const std::array<std::size_t, LagrangeInterpolator<DataType,N>::numberOfStages()> cStageSizes;

  static const std::array<std::size_t, LagrangeInterpolator<DataType,N>::numberOfStages()> cStageOffsets;
#endif
};

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_LAGRANGE_INTERPOLATOR_HPP_INCLUDED
