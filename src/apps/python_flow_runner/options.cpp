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

  registerOption<std::string>( "module-path,m", "Full pathe of the Python module to be loaded." );
  registerOption<std::string>( "python-class-name,c", "Name of the Python class (must be a subclass of visr.Component)." );
  registerOption<std::string>( "object-name,n", "Name of the Python class (must be a subclass of visr.Component)." );
  registerOption<std::string>( "positional-arguments,a", "Comma-separated list of positional options passed to the class constructor." );
  registerOption<std::string>( "keyword-arguments,k", "Comma-separated list of named (keyword) options passed to the class constructor." );
}

Options::~Options() = default;

} // namespace pythonflowrunner
} // namespace apps
} // namespace visr
