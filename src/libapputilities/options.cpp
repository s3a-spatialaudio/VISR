/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "options.hpp"

#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>

namespace visr
{
namespace apputilities
{

Options::Options()
{
  registerOption<bool>( "help,h", "Show help and usage information." );
  registerOption<bool>( "version,v", "Display version information." );
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

} // namespace apputilities
} // namespace visr
