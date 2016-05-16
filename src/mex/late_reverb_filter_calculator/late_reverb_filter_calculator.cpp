/* Copyright Institute of Sound and Vibration Research - All Rights reserved */

#include "signal_flow.hpp"

#include <libmexsupport/export_symbol.hpp>
#include <libmexsupport/mex_wrapper.hpp>

// Access the default subband number
#include <libobjectmodel/point_source_with_reverb.hpp>

#include <libril/constants.hpp>


#include <mex.h> 
#include <matrix.h>

#include <ciso646>
#include <string>

static char const * usage()
{
  return "Usage: output = late_reverb_filter_calculator( objectVector, lateReverbLength [, numberOfSubbands] )";
}

VISR_MEXSUPPORT_EXPORT_SYMBOL
void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[] )
{
  using namespace visr;
  using namespace visr::mex::late_reverb_filter_calculator;
  try
  {
    if( (nrhs < 2) or( nrhs > 3 ) )
    {
      mexErrMsgIdAndTxt( "visr:mexInterface:inputArgumentCount", usage() );
    }
    if( nlhs != 1 )
    {
      mexErrMsgIdAndTxt( "visr:mexInterface:outputArgumentCount", usage() );
    }
    if( (not mxIsChar( prhs[0] )) or ( (mxGetN(prhs[0])!=1) and (mxGetM(prhs[0])!=1)) )
    {
      mexErrMsgIdAndTxt( "visr:mexInterface:ArgumentType",
        "The parameter \"objectVector\" must be a char vector" );
    }
    std::string const objVectorStr( mxArrayToString( prhs[0] ) );

    if( (not( mxGetN( prhs[1]) == 1 and mxGetM( prhs[1] ) == 1 )) or( not mxIsDouble( prhs[1] ) ) )
    {
      mexErrMsgIdAndTxt( "visr:mexInterface:ArgumentType",
        "The parameter \"lateReverbLength\" must be a scalar double value." );
    }
    float const reverbLength = static_cast<float>(mxGetScalar( prhs[1] ) );
    std::size_t const periodSize = 512; // This is a dummy value, we don't need a period size, as there is no audio.

    ril::SamplingFrequencyType const samplingFrequency = 48000; // Same.

    SignalFlow flow( reverbLength, objectmodel::PointSourceWithReverb::cNumberOfSubBands,
                     periodSize, samplingFrequency );

    rcl::LateReverbFilterCalculator::LateFilterMessageQueue outputFilters;
    flow.process( objVectorStr, outputFilters );

    // Return the calculated filters
    std::size_t const numFilters = outputFilters.numberOfElements();
    if( numFilters == 0 )
    {
      plhs[0] = mxCreateDoubleMatrix( 0, 0, mxREAL );
    }
    else
    {
      for( std::size_t filterIdx( 0 ); filterIdx < numFilters; ++filterIdx )
      {
        std::pair<std::size_t, std::vector<ril::SampleType> > const & ir = outputFilters.nextElement( );
        if( filterIdx == 0 )
        {
          std::size_t const irLength = ir.second.size();
          plhs[0] = mxCreateDoubleMatrix( irLength, numFilters, mxREAL );
        }
        std::copy( ir.second.begin(), ir.second.end(), mxGetPr( plhs[0] ) );
        outputFilters.popNextElement();
      }
    }

  }
  catch( std::exception const & e )
  {
    mexErrMsgIdAndTxt( "visr:mexInterface:runtimeError", e.what() );
  }
}
