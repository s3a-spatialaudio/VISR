/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MAXMSPEXTERNALS_MATRIX_CONVOLVER_OPTIONS_HPP_INCLUDED
#define VISR_MAXMSPEXTERNALS_MATRIX_CONVOLVER_OPTIONS_HPP_INCLUDED

#include <libapputilities/options.hpp>

namespace visr
{
namespace maxmsp
{
namespace matrix_convolver
{

/**
 * Option definition for the matrix_convolver~ Max/MSP external.
 * This is a stripped-down version of the apps/matrix_convolver real-time version.
 * where any parts already handles by Max (audio backend, block size) have been removed.
 * The options are still provided, but are not used
 */
class Options: public apputilities::Options
{
public:
  Options();

  ~Options();
};

} // namespace matrix_convolver
} // namespace maxmsp
} // namespace visr

#endif // #ifndef VISR_MAXMSPEXTERNALS_MATRIX_CONVOLVER_OPTIONS_HPP_INCLUDED
