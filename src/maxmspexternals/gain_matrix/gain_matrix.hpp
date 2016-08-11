/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <maxmspexternals/libmaxsupport/external_base.hpp>
#include <maxmspexternals/libmaxsupport/signal_flow_wrapper.hpp>

#include <libsignalflows/gain_matrix.hpp>

// Some loony (Max?) obviously defines max, thus hiding any C++/STL functions of the same name.
#undef max
#undef min // same for min(), although there is no Min/MSP around.

#include <libpml/matrix_parameter.hpp>

#include <cstddef>
#include <memory>

// Some loony (Max?) obviously defines max, thus hiding any C++/STL functions of the same name.
#undef max
#undef min // same for min(), although there is no Min/MSP around.

namespace visr
{
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
  long mPeriod;
  std::size_t mNumberOfInputs;
  std::size_t mNumberOfOutputs;  
  std::size_t mInterpolationSteps;

  std::unique_ptr<signalflows::GainMatrix> mFlow;
  std::unique_ptr<maxmsp::SignalFlowWrapper<double> > mFlowWrapper;

  pml::MatrixParameter<ril::SampleType> mGains;
};

} // namespace maxmsp
} // namespace visr
