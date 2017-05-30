/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "lagrange_interpolator.hpp"

namespace visr
{
namespace efl
{

template< typename DataType, std::size_t N>
/*static*/ const std::array<std::size_t, LagrangeInterpolator<DataType,N>::numberOfStages()>
LagrangeInterpolator<DataType,N>::cStageSizes; // { /*LagrangeInterpolator<DataType,N>::generateStageSizes()*/ };

template< typename DataType, std::size_t N>
/*static*/ const std::array<std::size_t, LagrangeInterpolator<DataType,N>::numberOfStages()>
LagrangeInterpolator<DataType,N>::cStageOffsets; //( /*LagrangeInterpolator<DataType,N>::generateStageOffsets()*/ );


template< typename DataType, std::size_t N>
LagrangeInterpolator<DataType,N>::LagrangeInterpolator()
  : cOffsets( generateOffsets() )
  , cScaleFactors( generateScaleFactors() )
#if 0
  , cStageSizes( generateStageSizes() )
  , cStageOffsets( generateStageOffsets() )
#endif
{

}

template< typename DataType, std::size_t N>
void LagrangeInterpolator<DataType,N>::
calculateCoefficients( DataType mu, std::array<DataType,N+1> & result ) const
{

}

template< typename DataType, std::size_t N>
/*static*/ DataType LagrangeInterpolator<DataType,N>::offset( std::size_t idx )
{
  return static_cast<DataType>(idx)-static_cast<DataType>(0.5)*(static_cast<DataType>(N));
}

template< typename DataType, std::size_t N>
/*static*/ DataType LagrangeInterpolator<DataType,N>::scaleFactor( std::size_t idx )
{
  DataType const currOffset = offset(idx);
  DataType acc{ static_cast<DataType>(1.0) };
  for( std::size_t runIdx(0); runIdx <= N; ++runIdx )
  {
    if( runIdx != idx )
    {
      acc *= offset(runIdx) - currOffset;
    }
  }
  return static_cast<DataType>(1.0)/static_cast<DataType>(acc);
}

template< typename DataType, std::size_t N>
/*static*/ std::array<DataType, N+1> LagrangeInterpolator<DataType,N>::generateOffsets()
{
  std::array<DataType, N+1> offsets;
  for( std::size_t idx(0); idx <= N; ++idx )
  {
    offsets[idx] = offset(idx);
  }
  return offsets;
}

template< typename DataType, std::size_t N>
/*static*/ std::array<DataType, N+1> LagrangeInterpolator<DataType,N>::generateScaleFactors()
{
  std::array<DataType, N+1> scaleFactors;
  for( std::size_t idx(0); idx <= N; ++idx )
  {
    scaleFactors[idx] =  scaleFactor(idx);
  }
  return scaleFactors;
}

template< typename DataType, std::size_t N>
std::array<std::size_t, LagrangeInterpolator<DataType, N>::numberOfStages()> visr::efl::LagrangeInterpolator<DataType, N>::generateStageSizes()
{
  std::array<std::size_t, LagrangeInterpolator<DataType,N>::numberOfStages()> stageSizes;
  for( std::size_t idx(0); idx < numberOfStages(); ++idx )
  {
    stageSizes[idx] =  stageSize(idx);
  }
  return stageSizes;
}


template< typename DataType, std::size_t N>
/*static*/ std::array<std::size_t, LagrangeInterpolator<DataType,N>::numberOfStages()> LagrangeInterpolator<DataType,N>::generateStageOffsets()
{
  std::array<size_t, LagrangeInterpolator<DataType,N>::numberOfStages()> stageSizes;
  for( std::size_t idx(0); idx < numberOfStages(); ++idx )
  {
    stageSizes[idx] =  stageSize(idx);
  }
  return stageSizes;
}

// Explicit instantiations
template class LagrangeInterpolator<float, 1>;
template class LagrangeInterpolator<float, 3>;
template class LagrangeInterpolator<float, 5>;
template class LagrangeInterpolator<float, 7>;

} // namespace efl
} // namespace visr
