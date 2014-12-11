/* Copyright Institute of Sound and Vibration Research - All Rights reserved */

#include "mex_wrapper.hpp"
#include "signal_flow.hpp"

#include <libril/constants.hpp>

// for the moment, do some basic tests in the main mex function
#include <libefl/basic_matrix.hpp>
#include <librbbl/gain_matrix.hpp>

#include <mex.h> 
#include <matrix.h>

#include <ciso646>
#include <string>

static char const * usage()
{
  return "Usage: output = feedthrough( input [, parameterMessages], blockLength, samplingFrequency )";
}

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[] )
{
  using namespace visr;
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
#if 0
    // create an empty matrix
    efl::BasicMatrix<float> m1( ril::cVectorAlignmentSamples );

    // create a zero-initialised matrix of size 3x5
    efl::BasicMatrix<float> m2( 3, 5, ril::cVectorAlignmentSamples );

    // create a matrix from an initialiser list.
    efl::BasicMatrix<float> m3( 3, 5, {{ 0, 1, 2, 3, 4 }, { 5, 6, 7, 8, 9 }, { 10, 11, 12, 13, 14 } }, ril::cVectorAlignmentSamples );

    m1.resize( 3, 5 );

    m1.fillValue( 27.42f );

    m1.copy( m3 );

    m2.swap(m1);

    rbbl::GainMatrix<float> g1( 3, 5, periodSize, 4096, 0.0, ril::cVectorAlignmentSamples );

#else
    const std::size_t cNumberOfInputs = 2;
    const std::size_t cNumberOfOutputs = 8;
    const std::size_t cInterpolationLength = 4 * periodSize;

    mex::gain_matrix::SignalFlow flow( cNumberOfInputs, cNumberOfOutputs, cInterpolationLength, periodSize, samplingFrequency );
    flow.setup();

    mex::MexWrapper mexWrapper( flow, prhs[0], plhs[0],
                                hasParameterArg ? prhs[1] : nullptr );

    mexWrapper.process();
#endif
  }
  catch( std::exception const & e )
  {
    mexErrMsgIdAndTxt( "visr:mexInterface:runtimeError", e.what() );
  }
}
