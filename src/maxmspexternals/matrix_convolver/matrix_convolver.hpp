/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MAXMSP_MATRIX_CONVOLVER_MATRIX_CONVOLVER_HPP_INCLUDED
#define VISR_MAXMSP_MATRIX_CONVOLVER_MATRIX_CONVOLVER_HPP_INCLUDED


// these parameters could easily go into a private implementation object.
#include <libpml/matrix_parameter.hpp>

#include <librbbl/index_sequence.hpp>
#include <librbbl/filter_routing.hpp>

#include <librcl/fir_filter_matrix.hpp>

// We have to include these files last because they pull in the Max/MSP headers which do some very nasty 
// stuff such as defining macros min and max.
#include <maxmspexternals/libmaxsupport/external_base.hpp>
#include <maxmspexternals/libmaxsupport/signal_flow_wrapper.hpp>

#include <cstddef>
#include <memory>

namespace visr
{
namespace maxmsp
{
namespace matrix_convolver
{

class MatrixConvolver: public visr::maxmsp::ExternalBase
{
public:

  explicit  MatrixConvolver( t_pxobject & maxProxy, short argc, t_atom *argv );

  ~MatrixConvolver();

  /*virtual*/ void initDsp( t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags) override;

  /*virtual*/ void perform( t_object *dsp64, double **ins,
                            long numins, double **outs, long numouts,
                            long sampleframes, long flags, void *userparam) override;

  /*virtual*/ void assist( void *b, long msg, long arg, char *dst ) override;


  /*virtual*/ void getFloat( double f ) override;

private:
  /**
   ß*
   */
  std::unique_ptr<SignalFlowContext> mContext;

  std::size_t mNumberOfInputs;
  std::size_t mNumberOfOutputs;  
  
  std::size_t mMaxFilterLength;
  std::size_t mNumMaxFilters;

  rbbl::FilterRoutingList mRoutings;

  std::string mFilterList;
  rbbl::IndexSequence mIndexOffsets;

  std::string mFftLibrary;

  std::unique_ptr<rcl::FirFilterMatrix> mFlow;
  std::unique_ptr<SignalFlowWrapper<double> > mFlowWrapper;

  pml::MatrixParameter<SampleType> mGains;
};

} // namespace matrix_convolver
} // namespace maxmsp
} // namespace visr

#endif // #ifndef VISR_MAXMSP_MATRIX_CONVOLVER_MATRIX_CONVOLVER_HPP_INCLUDED
