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

namespace visr
{
namespace rcl
{

ReverbParameterCalculator::ReverbParameterCalculator( ril::AudioSignalFlow& container, char const * name )
 : AtomicComponent( container, name )
 , mMaxNumberOfObjects( 0 )
 , mObjectInput( *this, "objectInput", pml::EmptyParameterConfig() )
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
                                       std::size_t numLateReflectionSubBandFilters )
{
  mMaxNumberOfObjects=numberOfObjects;

}

/**
* The process function.
* It takes a vector of objects as input and calculates a vector of output gains.
*/
void ReverbParameterCalculator::process()
{
  int numReverbObjectsFound = 0;
  pml::ObjectVector const & objects = mObjectInput.data();
  for( objectmodel::ObjectVector::value_type const & objEntry : objects )
  {

    objectmodel::Object const & obj = *(objEntry.second);
    if( obj.numberOfChannels() != 1 )
    {
      std::cerr << "ReverbParameterCalculator: Only monaural object types are supported at the moment." << std::endl;
      continue;
    }

    objectmodel::ObjectTypeId const ti = obj.type();
    // Process reverb objects, ignore others
    switch( ti )
    {
    case objectmodel::ObjectTypeId::PointSourceWithReverb:
    {
      ++numReverbObjectsFound;
      if( numReverbObjectsFound > mMaxNumberOfObjects )
      {
        std::cerr << "ReverbParameterCalculator: max number of reverb objects exceeded." << std::endl;
        break;
      }
      processInternal( objects );
      break;
    }
    default:
    {
    }

    }
  } //for( objectmodel::ObjectVector::value_type const & objEntry : objects )
} //process


void ReverbParameterCalculator::processInternal(const objectmodel::ObjectVector & objects)
{
    std::cerr << "internal processing" << std::endl;
}

} // namespace rcl
} // namespace visr
