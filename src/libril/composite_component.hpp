/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_COMPOSITE_COMPONENT_HPP_INCLUDED
#define VISR_COMPOSITE_COMPONENT_HPP_INCLUDED

#include "component.hpp"
#include "channel_list.hpp"
#include "export_symbols.hpp"

#include <memory>
#include <vector>

namespace visr
{

// Forward declarations
class SignalFlowContext;

namespace impl
{
class CompositeComponentImplementation;
}

/**
 *
 *
 */
class  VISR_CORE_LIBRARY_SYMBOL CompositeComponent: public Component
{
public:
  /**
   * Making the type known inside CompositeConponent and derived classes
   * These are convenience aliases to make the syntax in derived signal flows more concise.
   * @note: This also means that we include the ChannelList/ChannelRange definition in this header, as these classes become part 
   * the CompositeComponent interface.
   */
  //@{
  using ChannelRange = visr::ChannelRange;
  using ChannelList = visr::ChannelList;
  //@}

  explicit CompositeComponent( SignalFlowContext& context,
                               char const * name,
                               CompositeComponent * parent = nullptr );

  /**
   * Destructor
   */
  ~CompositeComponent();

  /**
   * The number of contained components (not including the composite itself).
   * This method considers only atomic and composite components at the next level,
   * i.e., not recursively.
   */
  std::size_t numberOfComponents() const;

  /**
   * Return a reference to the internal data structures holding ports and contained components.
   * From the user point of view, these data structure is opaque and unknown.
   * @todo Improve name ('implementation' does not really fit)
   */
  impl::CompositeComponentImplementation & implementation();

  /**
  * Return a reference to the internal data structures holding ports and contained components, const version.
  * From the user point of view, these data structure is opaque and unknown.
  * @todo Improve name ('implementation' does not really fit)
  */
  impl::CompositeComponentImplementation const & implementation() const;

// protected:

  /**
   * TODO: Should we add an explicit addComponent() method instead of the implicit registration?
   */
  
  /**
   * Register a connection between parameter ports (both real ports of contained components or external placeholder ports.

   * @throw std::invalid_argument if a specified component or port does not exist.
   */
  void registerParameterConnection( char const * sendComponent,
    char const * sendPort,
    char const * receiveComponent,
    char const * receivePort );

  void registerParameterConnection( ParameterPortBase & sender,
                                    ParameterPortBase & receiver );


  void audioConnection( char const * sendComponent,
                        char const * sendPort,
                        ChannelList const & sendIndices,
                        char const * receiveComponent,
                        char const * receivePort,
                                ChannelList const & receiveIndices );

  void audioConnection( AudioPortBase & sendPort,
                                ChannelList const & sendIndices,
                                AudioPortBase & receivePort,
                                ChannelList const & receiveIndices );

  void audioConnection( AudioPortBase & sendPort,
                                AudioPortBase & receivePort );
private:
};

} // namespace visr

#endif // #ifndef VISR_COMPOSITE_COMPONENT_HPP_INCLUDED
