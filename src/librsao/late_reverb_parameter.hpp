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

  class LateReverbParameter: public TypedParameterBase<LateReverbParameter, pml::EmptyParameterConfig, detail::compileTimeHashFNV1(sLateReverbParameterName) >
{
public:
  LateReverbParameter();

  explicit LateReverbParameter( ParameterConfigBase const & config );

  /**
   * Copy constructor.
   * Also acts as default constructor.
   */
  explicit LateReverbParameter( pml::EmptyParameterConfig const & config );

  explicit LateReverbParameter( std::size_t index,
                                objectmodel::PointSourceWithReverb::LateReverb const & params );

  virtual ~LateReverbParameter() override;

  objectmodel::PointSourceWithReverb::LateReverb const & getReverbParameters()
  {
    return mParams;
  }

  void setReverbParameters( objectmodel::PointSourceWithReverb::LateReverb const & newParams )
  {
    mParams = newParams;
  }

  std::size_t index() const { return mIndex; }

  void setIndex( std::size_t newIndex ) { mIndex = newIndex; }

private:
  std::size_t mIndex;

  objectmodel::PointSourceWithReverb::LateReverb mParams;
};

} // namespace rsao
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::rsao::LateReverbParameter, visr::rsao::LateReverbParameter::staticType(), visr::pml::EmptyParameterConfig )

#endif // VISR_RSAO_LATE_REVERB_PARAMETER_HPP_INCLUDED
