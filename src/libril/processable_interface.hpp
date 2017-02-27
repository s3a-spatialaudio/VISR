/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PROCESSABLE_INTERFACE_HPP_INCLUDED
#define VISR_PROCESSABLE_INTERFACE_HPP_INCLUDED

namespace visr
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

} // namespace visr

#endif // #ifndef VISR_PROCESSABLE_INTERFACE_HPP_INCLUDED
