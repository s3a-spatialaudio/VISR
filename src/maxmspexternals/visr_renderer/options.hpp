/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MAXMSP_VISR_RENDERER_OPTIONS_HPP_INCLUDED
#define VISR_MAXMSP_VISR_RENDERER_OPTIONS_HPP_INCLUDED

#include <libapputilities/options.hpp>

namespace visr
{
namespace maxmsp
{
namespace visr_renderer
{

class Options: public apputilities::Options
{
public:
  Options();

  ~Options();
};

} // namespace visr_renderer
} // namespace maxmsp
} // namespace visr

#endif // #ifndef VISR_MAXMSP_VISR_RENDERER_OPTIONS_HPP_INCLUDED
