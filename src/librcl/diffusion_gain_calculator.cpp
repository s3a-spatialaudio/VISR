/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "diffusion_gain_calculator.hpp"

#include <libefl/basic_matrix.hpp>
#include <libefl/basic_vector.hpp>

#include <libobjectmodel/object_vector.hpp>
// Note: At the moment, all supported source types are translated directly in this file
// TODO: For the future, consider moving this to another location.
#include <libobjectmodel/diffuse_source.hpp>
#include <libobjectmodel/point_source_with_diffuseness.hpp>


#include <ciso646>
#include <iostream>

namespace visr
{
namespace rcl
{

DiffusionGainCalculator::DiffusionGainCalculator( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
 , mNumberOfObjectChannels( 0 )
{
}

DiffusionGainCalculator::~DiffusionGainCalculator()
{
}

void DiffusionGainCalculator::setup( std::size_t numberOfObjectChannels )
{
  mNumberOfObjectChannels = numberOfObjectChannels;
}

void DiffusionGainCalculator::process( objectmodel::ObjectVector const & objects, efl::BasicMatrix<CoefficientType> & gainVector )
{
  if( (gainVector.numberOfRows() != 1 ) or( gainVector.numberOfColumns() != mNumberOfObjectChannels ) )
  {
    throw std::invalid_argument( "DiffusionGainCalculator::process(): The gainVector argument must be a 1 x numberOfObjectChannels matrix." );
  }
  gainVector.zeroFill();
  processInternal( objects, gainVector.row( 0 ) );
}

void DiffusionGainCalculator::process( objectmodel::ObjectVector const & objects, efl::BasicVector<CoefficientType> & gainVector )
{
  if( (gainVector.size() != mNumberOfObjectChannels) )
  {
    throw std::invalid_argument( "DiffusionGainCalculator::process(): The gainVector argument must be a vectoor with numberOfObjectChannels elements." );
  }
  gainVector.zeroFill( );
  processInternal( objects, gainVector.data( ) );
}

void DiffusionGainCalculator::processInternal( objectmodel::ObjectVector const & objects, CoefficientType * gains )
{
  // For the moment, we assume that the audio channels of the objects are identical to the final channel numbers.
  // Any potential re-routing will be added later.
  for( objectmodel::ObjectVector::value_type const & objEntry : objects )
  {
    objectmodel::Object const & obj = *(objEntry.second);
    // Pre-check to handle only monaural objects here. The fine-grained disambiguation between supported object types happens later.
    if( obj.numberOfChannels() != 1 )
    {
      continue;
    }
    objectmodel::Object::ChannelIndex const channelId = obj.channelIndex( 0 );
    if( channelId >= mNumberOfObjectChannels )
    {
      std::cerr << "DiffusionGainCalculator: Channel index \"" << channelId << "\" of object id#" << objEntry.first
                << "exceeds number of channels (" << mNumberOfObjectChannels << ")." << std::endl;
      continue;
    }

    objectmodel::ObjectTypeId const ti = obj.type();

    // For the moment, we treat the two supported source type here.
    // @todo find a proper abstraction to handle many source types.
    switch( ti )
    {
    case objectmodel::ObjectTypeId::DiffuseSource:
    {
      gains[channelId] = obj.level();
      break;
    }
    case objectmodel::ObjectTypeId::PointSourceWithDiffuseness:
    {
      objectmodel::PointSourceWithDiffuseness const & psdSrc = dynamic_cast<objectmodel::PointSourceWithDiffuseness const &>(obj);
      gains[channelId] = psdSrc.diffuseness() * psdSrc.level();
      break;
    }
    default:
    // Other source types, including hitherto unknown, are set to zero diffuseness.
    gains[channelId] = static_cast<objectmodel::LevelType>(0.0f);
    }
  } // for( objectmodel::ObjectVector::value_type const & objEntry : objects )
}

} // namespace rcl
} // namespace visr
