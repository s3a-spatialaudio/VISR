/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "options.hpp"

#include <libvisr/constants.hpp> // for SampleType

#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>

namespace visr
{
namespace apps
{
namespace gain_matrix
{

Options::Options()
{
  registerOption<bool>( "help,h", "Show help and usage information." );
  registerOption<bool>( "version,v", "Display version information." );

  registerOption<std::string>( "audio-backend,D", "The audio backend." );
  registerOption<std::size_t>( "sampling-frequency,r", "Sampling frequency [Hz]" );
  registerOption<std::size_t>( "period,p", "Period (blocklength) [Number of samples per audio block]" );

  registerOption<std::string>( "matrix,m", "Initial gain matrix" );
  registerOption<std::string>( "matrix-file,f", "Initial gain matrix as a text file." );
  registerOption<SampleType>( "global-gain,g", "Overall gain modification (in dB) applied to the initial matrix." );
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

} // namespace gain_matrix
} // namespace apps
} // namespace visr
