/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_PYTHON_FLOW_RUNNER_OPTIONS_HPP_INCLUDED
#define VISR_APPS_PYTHON_FLOW_RUNNER_OPTIONS_HPP_INCLUDED

#include <libapputilities/options.hpp>

#include <iosfwd>
#include <string>

namespace visr
{
namespace apps
{
namespace pythonflowrunner
{

class Options: public apputilities::Options
{
public:
  Options();

  ~Options();
};
  
} // namespace pythonflowrunner
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_PYTHON_FLOW_RUNNER_OPTIONS_HPP_INCLUDED
