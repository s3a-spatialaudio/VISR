/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_BASELINE_RENDERER_OPTIONS_HPP_INCLUDED
#define VISR_APPS_BASELINE_RENDERER_OPTIONS_HPP_INCLUDED

#include <libapputilities/options.hpp>

namespace visr
{
namespace apps
{
namespace baseline_renderer
{

class Options: public apputilities::Options
{
public:
  Options();

  ~Options();
};

<<<<<<< HEAD
=======
template< typename DataType >
inline DataType Options::getDefaultedOption( char const * optionName, DataType const & defaultValue) const
{
  return hasOption( optionName ) ? getOption<DataType>( optionName ) : defaultValue;
}

template< typename DataType >
inline DataType Options::getOption( char const * optionName ) const
{
  if( mVariablesMap.count( optionName ) != 1 )
  {
    throw std::logic_error( std::string( "The mandatory command line option \"" )
      + optionName + "\" must be specified exactly once." );
  }
  try
  {
    auto const val = mVariablesMap[optionName];
    DataType const d = val.as<DataType>( );
    return d;
  }
  catch( std::exception const & ex )
  {
    throw std::logic_error( std::string( "The value \"" ) // + mVariablesMap[optionName]
      + "\" of the command line option \"" + optionName
      + "\" could not be converted to the target type: " + ex.what( ) );
  }
}

template<typename DataType >
inline void Options::registerOption( char const * optionName, char const * description )
{
  mDescription.add_options()( optionName, boost::program_options::value<DataType>(), description );
}

template<>
inline void Options::registerOption<bool>( char const * optionName, char const * description )
{
  mDescription.add_options( )(optionName, boost::program_options::bool_switch(), description);
}

template<typename DataType >
inline void Options::registerPositionalOption( char const * optionName, int maxOccurrences, char const * description )
{
  registerOption< DataType >( optionName, description );
  mPositionalDescription.add( optionName, maxOccurrences );
}

>>>>>>> 3b8f1889c6e66d81bd3c390bfd7f720dbfdacaa7
} // namespace baseline_renderer
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_BASELINE_RENDERER_OPTIONS_HPP_INCLUDED
