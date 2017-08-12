/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/parameter_base.hpp>
#include <libril/parameter_factory.hpp>
#include <libril/signal_flow_context.hpp>

#include <librsao/late_reverb_parameter.hpp> 

#include <pybind11/pybind11.h>

namespace visr
{

namespace python
{
namespace reverbobject
{

void exportLateReverbParameter( pybind11::module& m )
{
  pybind11::class_<::visr::rsao::LateReverbParameter, ParameterBase>( m, "LateReverbParameter" )
    .def( pybind11::init<BarameterConfigBase const &>(),
        pybind11::arg( "config" ) )
    ;

}

#if 0
class VISR_RSAO_LIBRARY_SYMBOL LateReverbParameter: public TypedParameterBase<LateReverbParameter, pml::EmptyParameterConfig, detail::compileTimeHashFNV1(sLateReverbParameterName) >
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
}
#endif
  
} // namepace reverbobject
} // namespace python
} // namespace visr
