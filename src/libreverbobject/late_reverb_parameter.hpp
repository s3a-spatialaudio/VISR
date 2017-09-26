/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_REVERBOBJECT_LATE_REVERB_PARAMETER_HPP_INCLUDED
#define VISR_REVERBOBJECT_LATE_REVERB_PARAMETER_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/detail/compile_time_hash_fnv1.hpp>
#include <libvisr/parameter_type.hpp>
#include <libvisr/typed_parameter_base.hpp>

#include <libpml/empty_parameter_config.hpp>

#include <libobjectmodel/point_source_with_reverb.hpp>

#include <cstdint>
#include <iosfwd>
#include <istream>

namespace visr
{
namespace reverbobject
{

/**
 * Define a unique name for the parameter type.
 */
static constexpr const char* sLateReverbParameterName = "reverbobject::LateReverbParameter";

class VISR_REVERBOBJECT_LIBRARY_SYMBOL LateReverbParameter: public TypedParameterBase<LateReverbParameter, pml::EmptyParameterConfig, detail::compileTimeHashFNV1(sLateReverbParameterName) >
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

  objectmodel::PointSourceWithReverb::LateReverb const & getReverbParameters() const
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

} // namespace reverbobject
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::reverbobject::LateReverbParameter, visr::reverbobject::LateReverbParameter::staticType(), visr::pml::EmptyParameterConfig )

#endif // VISR_REVERBOBJECT_LATE_REVERB_PARAMETER_HPP_INCLUDED
