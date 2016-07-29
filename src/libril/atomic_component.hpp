/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_ATOMIC_COMPONENT_HPP_INCLUDED
#define VISR_LIBRIL_ATOMIC_COMPONENT_HPP_INCLUDED

#include "component.hpp"

#include <algorithm> // due to temporary definition of findPortEntry() in header.
#include <cstddef>
#include <string>
#include <vector>

namespace visr
{
namespace ril
{

/**
 *
 *
 */
class AtomicComponent: public Component
{
public:
  explicit AtomicComponent( SignalFlowContext& context,
                            char const * name,
                            CompositeComponent * parent = nullptr );

  virtual void process() = 0;

  /**
   *
   */
  ~AtomicComponent();

  /**
  * Query whether this component is composite.
  * @return false
  */
  virtual bool isComposite();
 };

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_ATOMIC_COMPONENT_HPP_INCLUDED
