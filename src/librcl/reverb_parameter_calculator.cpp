/* Copyright Institute of Sound and Vibration Research - All rights reserved */

// avoid annoying warning about unsafe STL functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

#include "reverb_parameter_calculator.hpp"

#include <libefl/basic_matrix.hpp>

#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/point_source_with_reverb.hpp>

#include <libpml/listener_position.hpp>

#include <boost/filesystem.hpp>

#include <ciso646>
#include <cstdio>

// for math utility functions (see implementations in the unnamed namespace below)

namespace visr
{
namespace rcl
{

ReverbParameterCalculator::ReverbParameterCalculator( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
 , mMaxNumberOfObjects( 0 )
{
}

ReverbParameterCalculator::~ReverbParameterCalculator()
{
}

void ReverbParameterCalculator::setup( panning::LoudspeakerArray const & arrayConfig,
  std::size_t numberOfObjects,
  std::size_t numberOfDiscreteReflectionsPerSource,
  std::size_t numBiquadSectionsReflectionFilters,
  ril::SampleType lateReflectionLengthSeconds,
  std::size_t numLateReflectionSubBandFilters,
  efl::BasicMatrix<ril::SampleType> const & lateReflectionSubbandFilters )
{
}

/**
* The process function.
* It takes a vector of objects as input and calculates a vector of output gains.
*/
void ReverbParameterCalculator::process( objectmodel::ObjectVector const & objects,
  pml::SignalRoutingParameter & signalRouting,
  pml::BiquadParameterMatrix<ril::SampleType> & biquadCoeffs,
  efl::BasicVector<ril::SampleType> & discreteReflGains,
  efl::BasicMatrix<ril::SampleType> & discretePanningMatrix,
  efl::BasicMatrix<ril::SampleType> & lateReverbFilters )
{

}

} // namespace rcl
} // namespace visr
