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
  explicit AtomicComponent( AudioSignalFlow& container, char const * name );

  /**
   *
   */
  ~AtomicComponent();
 };

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_ATOMIC_COMPONENT_HPP_INCLUDED
