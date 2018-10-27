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
namespace pythonflowrunner
{

Options::Options()
{
  registerOption<std::string>( "audio-backend,D", "The audio backend." );
  registerOption<std::size_t>( "sampling-frequency,f", "Sampling frequency [Hz]" );
  registerOption<std::size_t>( "period,p", "Period (blocklength) [Number of samples per audio block]" );

  registerOption<std::string>( "module-name,m", "Name of the Python module to be loaded (without path or extension)." );

  registerOption<std::string>( "python-class-name,c", "Name of the Python class (must be a subclass of visr.Component)." );
  registerOption<std::string>( "object-name,n", "Name of the Python class (must be a subclass of visr.Component)." );
  registerOption<std::string>( "positional-arguments,a", "Comma-separated list of positional options passed to the class constructor." );
  registerOption<std::string>( "keyword-arguments,k", "A sequence of named (keyword) arguments passed to the class constructor as a Python dictionary, e.g., "
    "\"{ 'arg1': val1, ... 'argN': valN }\"." );
    registerOption<std::string>( "module-search-path,d", "Optional path to search for the Python module (in addition to the default search path, i.e., sys.path incl. $PYTHONPATH). " 
    "To be specified as a comma-separated list of directories." );
  registerOption<std::string>( "audio-ifc-options", "Audio interface optional configuration." );
  registerOption<std::string>( "audio-ifc-option-file", "Audio interface optional configuration file." );
}

Options::~Options() = default;

} // namespace pythonflowrunner
} // namespace apps
} // namespace visr
