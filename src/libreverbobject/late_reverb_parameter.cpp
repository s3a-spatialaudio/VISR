/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "late_reverb_parameter.hpp"

#include <libvisr/parameter_config_base.hpp>

namespace visr
{
namespace reverbobject
{

LateReverbParameter::LateReverbParameter()
  : mIndex(0)
{}

LateReverbParameter::LateReverbParameter( ParameterConfigBase const & )
  : LateReverbParameter()
{
}

LateReverbParameter::LateReverbParameter( pml::EmptyParameterConfig const & /*config = EmptyParameterConfig()*/ )
{}

LateReverbParameter::~LateReverbParameter(){} //  = default;

LateReverbParameter::LateReverbParameter( std::size_t index,
                                          objectmodel::PointSourceWithReverb::LateReverb const & params )
  : mIndex( index )
  , mParams( params )
{}

} // namespace reverbobject
} // namespace visr
