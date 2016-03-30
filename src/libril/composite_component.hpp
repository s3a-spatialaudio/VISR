/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_COMPOSITE_COMPONENT_HPP_INCLUDED
#define VISR_LIBRIL_COMPOSITE_COMPONENT_HPP_INCLUDED

#include "component.hpp"

namespace visr
{
namespace ril
{

/**
 *
 *
 */
class CompositeComponent: public Component
{
public:
  friend class AudioInput;
  friend class AudioOutput;
  friend class AudioSignalFlow;

  explicit CompositeComponent( AudioSignalFlow& container, char const * name );

  /**
   * Destructor
   */
  ~CompositeComponent();
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_COMPOSITE_COMPONENT_HPP_INCLUDED
