/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "fir_filter_matrix.hpp"

#include <librbbl/multichannel_convolver_uniform.hpp>

#include <ciso646>

namespace visr
{
namespace rcl
{

FirFilterMatrix::FirFilterMatrix( ril::SignalFlowContext& context,
                                  char const * name,
                                  ril::CompositeComponent * parent /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mInput( "in", *this )
 , mOutput( "out", *this )
 , mConvolver()
{
}

FirFilterMatrix::~FirFilterMatrix()
{
}

void FirFilterMatrix::setup( std::size_t numberOfInputs,
                             std::size_t numberOfOutputs,
                             std::size_t filterLength,
                             std::size_t maxFilters,
                             std::size_t maxRoutings,
                             efl::BasicMatrix<SampleType> const & filters /*= efl::BasicMatrix<SampleType>()*/,
                             pml::FilterRoutingList const & routings /*= pml::FilterRoutingList()*/,
                             char const * fftImplementation /*= "default" */ )
{
  mInput.setWidth( numberOfInputs );
  mOutput.setWidth( numberOfOutputs );
  mConvolver.reset( new rbbl::MultichannelConvolverUniform<SampleType>(
    numberOfInputs, numberOfOutputs, period(),
    filterLength, maxRoutings, maxFilters,
    routings, filters, ril::cVectorAlignmentSamples, fftImplementation ) );
}

void FirFilterMatrix::process()
{
  SampleType const * const * inputSignals = mInput.getVector();
  SampleType * const * outputSignals = mOutput.getVector( );
  mConvolver->process( inputSignals, outputSignals, ril::cVectorAlignmentSamples );
}

void FirFilterMatrix::clearRoutings()
{
  mConvolver->clearRoutingTable();
}

void FirFilterMatrix::addRouting( std::size_t inputIdx, std::size_t outputIdx, std::size_t filterIdx, SampleType const gain )
{
  mConvolver->setRoutingEntry( rbbl::MultichannelConvolverUniform<SampleType>::RoutingEntry( inputIdx, outputIdx, filterIdx, gain ) );
}

void FirFilterMatrix::addRouting( pml::FilterRoutingParameter const & routing )
{
  mConvolver->setRoutingEntry( routing.inputIndex, routing.outputIndex, routing.filterIndex, routing.gainLinear );
}

void FirFilterMatrix::addRoutings( pml::FilterRoutingList const & routings )
{
  for( pml::FilterRoutingParameter const & entry : routings )
  {
    addRouting( entry );
  }
}

void FirFilterMatrix::setRoutings( pml::FilterRoutingList const & routings )
{
  clearRoutings();
  addRoutings( routings );
}

bool FirFilterMatrix::removeRouting( std::size_t inputIdx, std::size_t outputIdx )
{
  return mConvolver->removeRoutingEntry( inputIdx, outputIdx );
}

void FirFilterMatrix::clearFilters()
{
  mConvolver->clearFilters();
}

void FirFilterMatrix::setFilter( std::size_t filterIdx, SampleType const * const impulseResponse, std::size_t filterLength, std::size_t alignment /*=0*/ )
{
  mConvolver->setImpulseResponse( impulseResponse, filterLength, filterIdx, alignment );
}

void FirFilterMatrix::setFilters( efl::BasicMatrix<SampleType> const & filterSet )
{
  mConvolver->initFilters( filterSet );
}

} // namespace rcl
} // namespace visr
