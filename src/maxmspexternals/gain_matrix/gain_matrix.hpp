/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <maxmspexternals/libmaxsupport/external_base.hpp>
#include <maxmspexternals/libmaxsupport/signal_flow_wrapper.hpp>

#include <libsignalflows/gain_matrix.hpp>

#include <libpml/matrix_parameter.hpp>

#include <cstddef>
#include <limits>
#include <memory>

namespace visr
{

namespace ril
{
class SignalFlow;
}

namespace maxmsp
{

class GainMatrix: public visr::maxmsp::ExternalBase
{
public:

  explicit  GainMatrix( t_pxobject & maxProxy, short argc, t_atom *argv );

  ~GainMatrix();

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
  std::size_t mPeriod;
  std::size_t mNumberOfInputs;
  std::size_t mNumberOfOutputs;  
  std::size_t mInterpolationSteps;

  /**
   * Context object to provide initialisation information and to provide a runt-time interface for the components.
   * Must be a pointer, as it can be instantiated only in the initiDSP() method.
   */
  std::unique_ptr<ril::SignalFlowContext> mContext;

  std::unique_ptr<signalflows::GainMatrix> mFlow;
  std::unique_ptr<maxmsp::SignalFlowWrapper<double> > mFlowWrapper;

  pml::MatrixParameter<ril::SampleType> mGains;
};

} // namespace maxmsp
} // namespace visr
