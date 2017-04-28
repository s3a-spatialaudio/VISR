/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "add.hpp"

#include <libril/audio_input.hpp>

#include <libril/constants.hpp>

#include <libefl/vector_functions.hpp>

#include <memory>
#include <sstream>
#include <vector>

namespace visr
{
namespace rcl
{


Add::Add( SignalFlowContext const & context,
          char const * name,
          CompositeComponent * parent,
	  std::size_t width,
	  std::size_t numInputs )
 : AtomicComponent( context, name, parent )
 , mOutput( "out", *this, width )
{
  mInputs.reserve( numInputs );
  for( std::size_t run( 0 ); run < numInputs; ++run )
  {
    // the C++11 function std::to_string(std::size_t) would be nifty here, but is missing on GCC 4.8.3 on Cygwin for some reason
    std::stringstream inName;
    inName << "in" << run;
    std::string const & portName = inName.str();
    std::unique_ptr<AudioInput> newIn( new AudioInput( portName.c_str(), *this, width ) );
    mInputs.push_back(  std::move( newIn ) );
  }
}

Add::~Add()
{
}

void Add::process()
{
  const std::size_t numInputs = mInputs.size();
  const std::size_t cWidth = mOutput.width();
  if( numInputs == 0 ) // In this special case, the Add block works like a source of zeros
  {
    for( std::size_t sigIdx( 0 ); sigIdx < cWidth; ++sigIdx )
    {
      efl::ErrorCode res = efl::vectorZero( mOutput[sigIdx], period(), cVectorAlignmentSamples );
      if( res != efl::noError )
      {
        throw std::runtime_error( std::string( "Error during Add::process(): " ) + efl::errorMessage( res ) );
      }
    }
  }
  else if( numInputs == 1 )
  {
    for( std::size_t sigIdx( 0 ); sigIdx < cWidth; ++sigIdx )
    {
      efl::ErrorCode res = efl::vectorCopy( mInputs.at(0)->at( sigIdx ), mOutput[sigIdx], period( ), cVectorAlignmentSamples );
      if( res != efl::noError ) {
        throw std::runtime_error( std::string( "Error during Add::process(): " ) + efl::errorMessage( res ) );
      }
    }
  }
  else
  {
    for( std::size_t sigIdx( 0 ); sigIdx < cWidth; ++sigIdx )
    {
      efl::ErrorCode res = efl::vectorAdd( mInputs.at( 0 )->at( sigIdx ),
        mInputs.at( 1 )->at( sigIdx ),
        mOutput[sigIdx], period( ), cVectorAlignmentSamples );
      if( res != efl::noError )
      {
        throw std::runtime_error( std::string( "Error during Add::process(): " ) + efl::errorMessage( res ) );
      }
    }
    for( std::size_t inputIdx( 2 ); inputIdx < numInputs; ++inputIdx )
    {
      for( std::size_t sigIdx( 0 ); sigIdx < cWidth; ++sigIdx )
      {
        efl::ErrorCode res = efl::vectorAddInplace( mInputs.at( inputIdx )->at( sigIdx ),
          mOutput[sigIdx], period( ), cVectorAlignmentSamples );
        if( res != efl::noError )
        {
          throw std::runtime_error( std::string( "Error during Add::process(): " ) + efl::errorMessage( res ) );
        }
      }
    }
  }
}

} // namespace rcl
} // namespace visr
