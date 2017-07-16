/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_RSAO_LATE_REVERB_PARAMETER_HPP_INCLUDED
#define VISR_RSAO_LATE_REVERB_PARAMETER_HPP_INCLUDED


#include <libril/detail/compile_time_hash_fnv1.hpp>
#include <libril/parameter_type.hpp>
#include <libril/typed_parameter_base.hpp>

#include <libpml/empty_parameter_config.hpp>
#include <libobjectmodel/point_source_with_reverb.hpp>

#include <cstdint>
#include <iosfwd>
#include <istream>

namespace visr
{
namespace rsao
{

/**
 * Define a unique name for the parameter type.
 */
static constexpr const char* sLateReverbParameterName = "rsao::LateReverbParameter";

class LateReverbParameter: public TypedParameterBase<LateReverbParameter, EmptyParameterConfig, detail::compileTimeHashFNV1(sLateReverbParameterName) >
{
public:
  explicit LateReverbParameter( ParameterConfigBase const & config );

  /**
   * Also acts as default constructor.
   */
  explicit LateReverbParameter( EmptyParameterConfig const & config = EmptyParameterConfig() );

  virtual ~LateReverbParameter() override;

private:
  objectmodel::PointSourceWithReverb::LateReverb mCoeffs;
};


} // namespace pml
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::rsao::LateReverbParameter, visr::rsao::LateReverbParam::staticType(), visr::pml::EmptyParameterConfig )

#endif // VISR_RSAO_LATE_REVERB_PARAMETER_HPP_INCLUDED
