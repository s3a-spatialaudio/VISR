/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_PROCESSABLE_INTERFACE_HPP_INCLUDED
#define VISR_LIBRIL_PROCESSABLE_INTERFACE_HPP_INCLUDED

namespace visr
{
namespace ril
{

/**
 * Interface class for entities that have a parameterless process() function.
 * That includes AtomicComponents as well as other infrastructure elements created by the runtime system.
 */
class ProcessableInterface
{
public:
  virtual void process() = 0;
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_PROCESSABLE_INTERFACE_HPP_INCLUDED
