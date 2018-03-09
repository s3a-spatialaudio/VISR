/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "null_source.hpp"

#include <libefl/vector_functions.hpp>

namespace visr
{
namespace rcl
{


  NullSource::NullSource( SignalFlowContext const & context,
                          char const * name,
                          CompositeComponent * parent,
                          std::size_t width )
 : AtomicComponent( context, name, parent )
 , mOutput( "out", *this, width )
{
}

NullSource::~NullSource()
{
}

void NullSource::process()
{
  std::size_t const numInputs( mOutput.width() );
  for( std::size_t sigIdx( 0 ); sigIdx < numInputs; ++sigIdx )
  {
    efl::ErrorCode res = efl::vectorZero( mOutput[sigIdx], period(), cVectorAlignmentSamples );
    if( res != efl::noError )
    {
      throw std::runtime_error( std::string( "Error during NullSource::process(): " ) + efl::errorMessage( res ) );
    }
  }
}

} // namespace rcl
} // namespace visr
