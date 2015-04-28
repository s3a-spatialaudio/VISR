/* Copyright Institute of Sound and Vibration Research - All Rights reserved */

// #ifdef _MSC_VER 
#pragma warning(disable: 4996)
// #endif


#include "utility_functions.hpp"

#include <libefl/basic_matrix.hpp>
#include <libefl/basic_vector.hpp>

#include <librbbl/multichannel_convolver_uniform.hpp>

#include <mex.h> 
#include <matrix.h>

#include <ciso646>
#include <cmath>
#include <string>

static char const * usage()
{
  return "Usage: output = multichannel_convolver( input, numberOfOutputs, blockLength, filterLength, maxRoutings, maxFilters. routings, filters )";
}

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[] )
{
  using namespace visr;
  using namespace visr::mex::multichannel_convolver;
  using SampleType = double;
  try
  {
    static std::size_t const defaultAlignment = 8; // Default alignment (in number of elements) used for all user-allocated matrices.

    if( (nrhs < 8) or( nrhs > 8 ) )
    {
      mexErrMsgIdAndTxt( "visr:mexInterface:inputArgumentCount", usage() );
    }
    if( nlhs != 1 )
    {
      mexErrMsgIdAndTxt( "visr:mexInterface:outputArgumentCount", usage() );
    }

    std::size_t const numberOfInputSignals = mxGetN( prhs[0] );
    std::size_t const inputSignalLength = mxGetM( prhs[0] );
    double const * inputBasePtr = mxGetPr( prhs[0] );

    double const numberOfOutputsRaw = mxGetScalar( prhs[1] );
    if( (std::fmod( numberOfOutputsRaw, 1.0 ) != 0.0) or (numberOfOutputsRaw < 0.0))
    {
      mexErrMsgTxt( "The argument \"numberOfOutputs\" must be a positive integer." );
    }
    std::size_t const numberOfOutputs = static_cast<std::size_t>(numberOfOutputsRaw);

    double const blockLengthRaw = mxGetScalar( prhs[2] );
    if( (std::fmod( blockLengthRaw, 1.0 ) != 0.0) or ( blockLengthRaw < 1.0 ) )
    {
      mexErrMsgTxt( "The argument \"blockLength\" must be a positive integer." );
    }
    std::size_t const blockLength = static_cast<std::size_t>(blockLengthRaw);

    if( inputSignalLength % blockLength != 0 )
    {
      mexErrMsgTxt( "The length of the input signal must be an integer multiple of the block length." );
    }
    std::size_t const numBlocks = inputSignalLength / blockLength;

    double const filterLengthRaw = mxGetScalar( prhs[3] );
    if( (std::fmod( filterLengthRaw, 1.0 ) != 0.0) or( filterLengthRaw < 1.0 ) )
    {
      mexErrMsgTxt( "The argument \"filterLength\" must be a positive integer." );
    }
    std::size_t const filterLength = static_cast<std::size_t>(filterLengthRaw);

    double const maxRoutingsRaw = mxGetScalar( prhs[4] );
    if( (std::fmod( maxRoutingsRaw, 1.0 ) != 0.0) or( maxRoutingsRaw < 1.0 ) )
    {
      mexErrMsgTxt( "The argument \"maxRoutings\" must be a positive integer." );
    }
    std::size_t const maxRoutings = static_cast<std::size_t>(maxRoutingsRaw);

    double const maxFiltersRaw = mxGetScalar( prhs[5] );
    if( (std::fmod( maxFiltersRaw, 1.0 ) != 0.0) or( maxFiltersRaw < 1.0 ) )
    {
      mexErrMsgTxt( "The argument \"maxFilters\" must be a positive integer." );
    }
    std::size_t const maxFilters = static_cast<std::size_t>(maxFiltersRaw);


    std::vector<typename rbbl::MultichannelConvolverUniform<SampleType>::RoutingEntry> const routingTable =
    visr::mex::multichannel_convolver::createRoutingTable<SampleType>( prhs[6] );

    efl::BasicMatrix<SampleType> filterMtx( defaultAlignment );
    createFilterMatrix( prhs[7], filterMtx, filterLength );

    efl::BasicMatrix<SampleType> inputMatrix( numberOfInputSignals, blockLength, defaultAlignment );
    std::vector<SampleType const *> inputPointers( numberOfInputSignals );
    for( std::size_t chIdx( 0 ); chIdx < numberOfInputSignals; ++chIdx )
    {
      inputPointers[chIdx] = inputMatrix.row( chIdx );
    }
      
    efl::BasicMatrix<SampleType> outputMatrix(numberOfOutputs, blockLength, defaultAlignment);
    std::vector<SampleType *> outputPointers( numberOfOutputs );
    for( std::size_t chIdx( 0 ); chIdx < numberOfOutputs; ++chIdx )
    {
      outputPointers[chIdx] = outputMatrix.row( chIdx );
    }
    plhs[0] = mxCreateDoubleMatrix( numberOfOutputs, inputSignalLength, mxREAL );

    /*
      explicit MultichannelConvolverUniform( std::size_t numberOfInputs,
                                         std::size_t numberOfOutputs,
                                         std::size_t blockLength,
                                         std::size_t maxFilterLength,
                                         std::size_t maxRoutingPoints,
                                         std::size_t maxFilterEntries,
                                         std::vector<RoutingEntry> const & initialRoutings,
                                         efl::BasicMatrix<SampleType> const & initialFilters,
                                         std::size_t alignment = 0 );
    */
    rbbl::MultichannelConvolverUniform<SampleType> conv( numberOfInputSignals,
                                                         numberOfOutputs,
                                                         blockLength,
                                                         filterLength,
                                                         maxRoutings,
                                                         maxFilters,
                                                         routingTable,
                                                         filterMtx );

    for( std::size_t blockIdx( 0 ); blockIdx < numBlocks; ++blockIdx )
    {
    }
  }
  catch( std::exception const & e )
  {
    mexErrMsgIdAndTxt( "visr:mexInterface:runtimeError", e.what() );
  }
}
