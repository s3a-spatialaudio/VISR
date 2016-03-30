/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "null_source.hpp"

#include <libefl/vector_functions.hpp>

namespace visr
{
namespace rcl
{


NullSource::NullSource( ril::AudioSignalFlow& container, char const * name )
 : AtomicComponent( container, name )
 , mOutput( "out", *this )
{
}

NullSource::~NullSource()
{
}

void NullSource::setup( std::size_t width )
{
  mOutput.setWidth( width );
}

void NullSource::process()
{
  std::size_t const numInputs( mOutput.width() );
  for( std::size_t sigIdx( 0 ); sigIdx < numInputs; ++sigIdx )
  {
    efl::ErrorCode res = efl::vectorZero( mOutput[sigIdx], period(), ril::cVectorAlignmentSamples );
    if( res != efl::noError )
    {
      throw std::runtime_error( std::string( "Error during NullSource::process(): " ) + efl::errorMessage( res ) );
    }
  }
}

} // namespace rcl
} // namespace visr
