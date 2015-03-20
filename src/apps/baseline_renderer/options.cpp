/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "options.hpp"

#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>

namespace visr
{
namespace apps
{
namespace baseline_renderer
{

Options::Options()
{
  registerOption<bool>( "help,h", "Show help and usage information." );
  registerOption<bool>( "version,v", "Display version information." );

  registerOption<std::string>( "audio-backend,D", "The audio backend." );
  registerOption<std::size_t>( "sampling-frequency,f", "Sampling frequency [Hz]" );
  registerOption<std::size_t>( "period,p", "Period (blocklength) [Number of samples per audio block]" );

  registerPositionalOption<std::string>( "array-config,c", 1, "Loudspeaker array configuration file" );
  registerOption<std::size_t>( "input-channels,i", "Number of input channels for audio object signal" );
  registerOption<std::size_t>( "output-channels,o", "Number of audio output channels" );

  registerOption<std::string>( "output-routing", "Routing of logical renderer outputs to physical output channels. Overrides the channel configuration in the array configuration file.");

  registerOption<std::string>( "tracking", "Enable adaptation of the panning using visual tracking. Accepts the position of thee tracker in the format \"{ \"x\": <x in m>, \"y\": <y im m>, \"z\": <z in m> }\"." );

  registerOption<std::size_t>( "scene-port,r", "UDP port for receiving object metadata" );
}

Options::~Options()
{
}

Options::ParseResult Options::parse( int argc, char const * const * argv, std::ostream & errMsg )
{
  try
  {
    boost::program_options::store( boost::program_options::parse_command_line( argc, argv, mDescription ),
                                   mVariablesMap );
    notify( mVariablesMap );
  }
  catch( std::exception const & ex )
  {
    errMsg << ex.what();
    return ParseResult::Failure;
  }
  if( getOption<bool>( "help" ) )
  {
    return ParseResult::Help;
  }
  if( getOption<bool>( "version" ) )
  {
    return ParseResult::Version;;
  }
  return ParseResult::Success;
}
  
bool Options::hasOption( char const * optionName ) const
{
  return (mVariablesMap.count( optionName ) >= 1);
}

void Options::printDescription( std::ostream & out )
{
  out << mDescription << std::endl;
}

} // namespace scene_decoder
} // namespace apps
} // namespace visr
