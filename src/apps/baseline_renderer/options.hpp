/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_BASELINE_RENDERER_OPTIONS_HPP_INCLUDED
#define VISR_APPS_BASELINE_RENDERER_OPTIONS_HPP_INCLUDED

#include <boost/program_options.hpp>

#include <iosfwd>
#include <string>

namespace visr
{
namespace apps
{
namespace baseline_renderer
{

class Options
{
public:
  Options();

  ~Options();

  enum class ParseResult
  {
    Success,
    Failure,
    Version,
    Help
  };

  ParseResult parse( int argc, char const * const * argv, std::ostream & errMsg );

  bool hasOption( char const * optionName ) const;

  template< typename DataType >
  DataType getOption( char const * optionName ) const;

  template< typename DataType >
  DataType getDefaultedOption( char const * optionName, DataType const & defaultValue ) const;

  /**
   * Print the option description.
   * @param out The output stream to which the description is written to.
   */
  void printDescription( std::ostream & out );

protected:
  template<typename DataType >
  void registerOption( char const * optionName, char const * description );

  template<typename DataType >
  void registerPositionalOption( char const * optionName, int position, char const * description );

  boost::program_options::options_description mDescription;
  boost::program_options::positional_options_description mPositionalDescription;

  boost::program_options::variables_map mVariablesMap;
};

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
inline void Options::registerPositionalOption( char const * optionName, int position, char const * description )
{
  registerOption< DataType >( optionName, description );
  mPositionalDescription.add( optionName, position );
}

} // namespace baseline_renderer
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_BASELINE_RENDERER_OPTIONS_HPP_INCLUDED
