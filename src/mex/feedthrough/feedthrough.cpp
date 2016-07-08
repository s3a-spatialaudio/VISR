/* Copyright Institute of Sound and Vibration Research - All Rights reserved */

#include "signal_flow.hpp"

#include <libmexsupport/export_symbol.hpp>
#include <libmexsupport/mex_wrapper.hpp>

#include <libril/constants.hpp>

#include <mex.h> 
#include <matrix.h>

#include <ciso646>
#include <string>

namespace ril = visr::ril;

static char const * usage()
{
  return "Usage: output = feedthrough( input [, parameterMessages], blockLength, samplingFrequency )";
}

VISR_MEXSUPPORT_EXPORT_SYMBOL
void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[] )
{
  try
  {
    if( (nrhs < 3) or( nrhs > 4 ) )
    {
      mexErrMsgIdAndTxt( "visr:mexInterface:inputArgumentCount", usage() );
    }
    if( nlhs != 1 )
    {
      mexErrMsgIdAndTxt( "visr:mexInterface:outputArgumentCount", usage() );
    }

    if( mxIsComplex( prhs[0] ) or not( mxIsSingle( prhs[0] ) or mxIsDouble( prhs[0] ) ) )
    {
      mexErrMsgIdAndTxt( "visr:mexInterface:ArgumentType",
        "The parameter \"message\" mut be a matrix of type single or double" );
    }

    bool const hasParameterArg = (nrhs == 4);

    std::size_t const periodSizeParamIdx = hasParameterArg ? 2 : 1;
    std::size_t const samplingFreqParamIdx = hasParameterArg ? 3 : 2;

    if( (not mxIsNumeric( prhs[periodSizeParamIdx] ) or( mxGetN( prhs[periodSizeParamIdx] ) != 1 )
      or( mxGetM( prhs[periodSizeParamIdx] ) != 1 )) )
    {
      mexErrMsgIdAndTxt( "visr:mexInterface:argumentType",
        "The parameter \"period\" mut be a scalar numerical value" );
    }

    if( (not mxIsNumeric( prhs[samplingFreqParamIdx] ) or( mxGetN( prhs[samplingFreqParamIdx] ) != 1 )
      or( mxGetM( prhs[samplingFreqParamIdx] ) != 1 )) )
    {
      mexErrMsgIdAndTxt( "visr:mexInterface:argumentType",
        "The parameter \"samplingFrequency\" mut be a scalar numerical value" );
    }

    std::size_t const periodSize
      = static_cast<ril::SamplingFrequencyType>(mxGetScalar( prhs[periodSizeParamIdx] ));

    ril::SamplingFrequencyType const samplingFrequency
      = static_cast<ril::SamplingFrequencyType>(mxGetScalar( prhs[samplingFreqParamIdx] ));

    //double const * const input = mxGetPr( prhs[0] );
    //double * const outputPtr = mxGetPr( prhs[1] );

    visr::mex::feedthrough::SignalFlow flow( periodSize, samplingFrequency );
    flow.setup();

    visr::mexsupport::MexWrapper mexWrapper( flow, prhs[0], plhs[0],
					     hasParameterArg ? prhs[1] : nullptr );

    mexWrapper.process();
  }
  catch( std::exception const & e )
  {
    mexErrMsgIdAndTxt( "visr:mexInterface:runtimeError", e.what() );
  }
}
