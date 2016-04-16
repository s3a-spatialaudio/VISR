/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_BASELINE_RENDERER_OPTIONS_HPP_INCLUDED
#define VISR_APPS_BASELINE_RENDERER_OPTIONS_HPP_INCLUDED

#include <libapputilities/options.hpp>

#include <boost/program_options.hpp>

#include <iosfwd>
#include <string>

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

} // namespace scene_decoder
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_BASELINE_RENDERER_OPTIONS_HPP_INCLUDED
