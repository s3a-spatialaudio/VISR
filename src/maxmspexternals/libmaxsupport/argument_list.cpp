/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "argument_list.hpp"

namespace visr
{
namespace maxmsp
{

ArgumentList::ArgumentList( short argc, t_atom *argv )
{
  char * textPtr = nullptr;
  long textSize = 0;

  if( atom_gettext( argc, argv, &textSize, &textPtr, OBEX_UTIL_ATOM_GETTEXT_COMMA_DELIM )
        != MAX_ERR_NONE )
  {
    sysmem_freeptr( textPtr );
    throw std::invalid_argument( "ArgumentList: Getting text from Max atoms failed." );
  }
  std::string const textStr( textPtr );
  sysmem_freeptr( textPtr );
  textPtr = nullptr;

  boost::split( mArguments, textStr, boost::is_any_of( "," ) );
  for( std::string & v : mArguments )
  {
    boost::trim_if( v, boost::is_any_of( "\t " ) );
  }

  // Prepend the zeroth argument with a dummy value.
  mArguments.insert( mArguments.begin(), "zeroth argument" );

  // Fill mArgPointers with the char* pointers of the strings in mArguments.
  std::transform( mArguments.begin(), mArguments.end(), std::back_inserter( mArgPointers ),
        []( std::string const & v ) { return v.c_str(); } );
}

std::size_t ArgumentList::argc( ) const
{
  return mArguments.size( );
}

char const * const * ArgumentList::argv( ) const 
{
  return &mArgPointers[0];
}

} // namespace visr
} // namespace maxmsp
