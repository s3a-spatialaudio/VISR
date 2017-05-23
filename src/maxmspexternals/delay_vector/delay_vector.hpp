/* Copyright Institute of Sound and Vibration Research - All rights reserved */

/* Super-safe determination of the MAX define for setting the operating system. */
#ifdef __APPLE_CC__
#ifndef MAC_VERSION 
#define MAC_VERSION
#undef WIN_VERSION
#endif
#else
#ifdef _MSC_VER
#ifndef WIN_VERSION 
#define WIN_VERSION
#endif
#undef MAC_VERSION
#endif
#endif

#include <maxmspexternals/libmaxsupport/external_base.hpp>
#include <maxmspexternals/libmaxsupport/signal_flow_wrapper.hpp>

#include <libril/signal_flow_context.hpp>
#include <libril/parameter_output.hpp>

#include <libpml/double_buffering_protocol.hpp>
#include <libpml/vector_parameter.hpp>

#include <librcl/delay_vector.hpp>

#include <librrl/audio_signal_flow.hpp>

#include <cstddef>
#include <limits>
#include <memory>

namespace visr
{
namespace maxmsp
{

class DelayVector: public visr::maxmsp::ExternalBase
{
public:

  explicit DelayVector( t_pxobject & maxProxy, short argc, t_atom *argv );

  ~DelayVector();

  /*virtual*/ void initDsp( t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

  /*virtual*/ void perform( t_object *dsp64, double **ins,
                            long numins, double **outs, long numouts,
                            long sampleframes, long flags, void *userparam);

  /*virtual*/ void assist( void *b, long msg, long arg, char *dst );


  /*virtual*/ void getFloat( double f );

private:
  /**
   * The number of samples to be processed per call.
   * The type is chosen to be compatible to the parameter passed by the calling Max/MSP functions.
   */
  long mPeriod;
  std::size_t mNumberOfChannels;
  std::size_t mInterpolationSteps;
  rcl::DelayVector::InterpolationType mInterpolationType;

  /**
   * Context object to provide initialisation information and to provide a run-time interface for the components.
   * Must be a pointer, as it can be instantiated only in the initDSP() method.
   */
  std::unique_ptr<SignalFlowContext> mContext;

  std::unique_ptr<rcl::DelayVector> mComp;
 
  std::unique_ptr<maxmsp::SignalFlowWrapper<double> > mFlowWrapper;

  std::unique_ptr<rrl::AudioSignalFlow> mFlow;

  using ParameterType = pml::VectorParameter<SampleType>;

  using ParameterPortType = ParameterOutput<pml::DoubleBufferingProtocol, ParameterType >;
  
  ParameterPortType* mGainInput;
  ParameterPortType* mDelayInput;
};

} // namespace maxmsp
} // namespace visr
