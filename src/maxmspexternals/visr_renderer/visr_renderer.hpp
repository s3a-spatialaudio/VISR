/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MAXMSP_VISR_RENDERER_VISR_RENDERER_HPP_INCLUDED
#define VISR_MAXMSP_VISR_RENDERER_VISR_RENDERER_HPP_INCLUDED

// these parameters could easily go into a private implementation object.
#include <libpml/filter_routing_parameter.hpp>
#include <libpml/index_sequence.hpp>
#include <libpml/matrix_parameter.hpp>

// We have to include these files last because they pull in the Max/MSP headers which do some very nasty 
// stuff such as defining macros min and max.
#include <maxmspexternals/libmaxsupport/external_base.hpp>
#include <maxmspexternals/libmaxsupport/signal_flow_wrapper.hpp>

#include <cstddef>
#include <memory>

namespace visr
{
// Forward declarations
namespace panning
{
class LoudspeakerArray;
}
namespace signalflows
{
class BaselineRenderer;
};
namespace maxmsp
{
namespace visr_renderer
{

class VisrRenderer: public visr::maxmsp::ExternalBase
{
public:

  explicit  VisrRenderer( t_pxobject & maxProxy, short argc, t_atom *argv );

  ~VisrRenderer();

  /*virtual*/ void initDsp( t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

  /*virtual*/ void perform( t_object *dsp64, double **ins,
                            long numins, double **outs, long numouts,
                            long sampleframes, long flags, void *userparam);

  /*virtual*/ void assist( void *b, long msg, long arg, char *dst );


  /*virtual*/ void getFloat( double f );

private:
  /**
   * Flag to specify whether the configuration parsing in the constructor was successfull.
   * If not, the initDsp() method will return immediately with an error.
   * This is necessary because initDsp() is called regardless of whether the constructor fails.
   */
  bool mConfigInitialised;


  /**
   * The number of samples to be processed per call.
   * The type is chosen to be compatible to the parameter passed by the calling Max/MSP functions.
   */
  long mPeriod;
  std::size_t mNumberOfObjects;
  std::size_t mNumberOfOutputs;

  std::size_t mNumberOfEqSections;
  
  std::unique_ptr<panning::LoudspeakerArray> mArrayConfiguration;

  std::unique_ptr< efl::BasicMatrix<ril::SampleType> > mDiffusionFilters;

  std::string mTrackingConfiguration;

  std::size_t mSceneReceiverPort;

  std::unique_ptr<signalflows::BaselineRenderer> mFlow;
  std::unique_ptr<maxmsp::SignalFlowWrapper<double> > mFlowWrapper;
};

} // namespace visr_renderer
} // namespace maxmsp
} // namespace visr

#endif // #ifndef VISR_MAXMSP_VISR_RENDERER_VISR_RENDERER_HPP_INCLUDED
