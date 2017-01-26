/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_ATOMIC_COMPONENT_HPP_INCLUDED
#define VISR_LIBRIL_ATOMIC_COMPONENT_HPP_INCLUDED

#include "component.hpp"
#include "processable_interface.hpp"

#include <cstddef>

namespace visr
{
namespace ril
{

/**
 *
 *
 */
class AtomicComponent: public Component,
                       public ProcessableInterface
{
public:
  explicit AtomicComponent( SignalFlowContext& context,
                            char const * name,
                            CompositeComponent * parent = nullptr );

  virtual void process() override = 0;

  /**
   *
   */
  ~AtomicComponent();

  /**
  * Query whether this component is composite.
  * @return false
  */
  virtual bool isComposite() const override;
 };

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_ATOMIC_COMPONENT_HPP_INCLUDED
