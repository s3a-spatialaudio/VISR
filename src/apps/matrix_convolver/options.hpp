/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_MATRIX_CONVOLVER_OPTIONS_HPP_INCLUDED
#define VISR_APPS_MATRIX_CONVOLVER_OPTIONS_HPP_INCLUDED

#include <libapputilities/options.hpp>

namespace visr
{
namespace apps
{
namespace matrix_convolver
{

class Options: public apputilities::Options
{
public:
  Options();

  ~Options();
};

} // namespace matrix_convolver
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_MATRIX_CONVOLVER_OPTIONS_HPP_INCLUDED
