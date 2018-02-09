/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MAXMSP_MAXSUPPORT_ARGUMENT_LIST_HPP_INCLUDED
#define VISR_MAXMSP_MAXSUPPORT_ARGUMENT_LIST_HPP_INCLUDED 

#include "export_symbols.hpp"

#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>
#include <vector>

// Include the Max headers last, as they do some unpleasant stuff (redefinition of min,max,error,...)

/* Super-safe determination of the MAX define for setting the operating system. */
#ifdef __APPLE_CC__
#ifndef MAC_VERSION 
#define MAC_VERSION
#undef WIN_VERSION
#endif
#else
#ifdef _MSC_VER
#ifndef WIN_VERSION 
#define WIN_VERSION
#endif
#undef MAC_VERSION
#endif
#endif

#include <ext.h>
#include <ext_obex.h>
#include "z_dsp.h"

namespace visr
{
namespace maxmsp
{

  /**
  * Transform and wrap a MAX/MSP option list into a standard argument list such as 
  * used by the C/C++ main() function or accepted by boost::program_options.
  * This implies that all type information carried by the Max atoms is discarded and the atoms are returned into 
  * their string representation.
  * In addition, a dummy value is prepended at position zero, because program_options interprets the zeroth
  * arguments as the binary name (as it is in a standard-compliant option list).
  * 
  */
  class VISR_MAXSUPPORT_LIBRARY_SYMBOL ArgumentList
  {
  public:
    /**
     * Constructor, parses the max-style list into a C-style option list.
     * @param argc Number of arguments
     * @param argv Arguments, provided as array of Max atoms.
     */
    explicit ArgumentList( short argc, t_atom *argv );

    /**
     * Query the number of arguments.
     * @note The number is one larger than the number of Max/MSP arguments passed to the constructor, since a 'zeroth'
     * argument, which represents the binary name in a C-style option list, is prepended.
     */
    std::size_t argc() const;

    /**
     * Return the parsed argument as a c-style pointer array (as in a standard option list)
     * @note A zeroth argument with the dummy value "zeroth argument" is prepended to meet the 
     * conventions of a C-style argument list (zeroth argument is the binary name).
     */
    char const * const * argv() const;

  private:
    /**
     * The parsed arguments in a string representation.
     */
    std::vector< std::string > mArguments;

    /**
     * Vector of C-style char pointers to the content of mArgument.
     */
    std::vector<char const *> mArgPointers;
  };

} // namespace maxmsp
} // namespace visr

#endif // VISR_MAXMSP_MAXSUPPORT_ARGUMENT_LIST_HPP_INCLUDED 
