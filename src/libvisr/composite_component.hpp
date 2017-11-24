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
 * Base class for processing components that are composed of other components (atomic and composite).
 * In this way, processing components can be structured hierarchically.
 * Composite components store the contained sub-components, external audio and parameter ports, and conenctions between the ports.
 */
class  VISR_CORE_LIBRARY_SYMBOL CompositeComponent: public Component
{
public:
  /**
   * Making the types for defining audio connections known inside CompositeConponent and derived classes
   * These are convenience aliases to make the syntax in derived signal flows more concise.
   * @note: This also means that we include the ChannelList/ChannelRange definition in this header, as these classes become part 
   * the CompositeComponent interface.
   */
  //@{
  using ChannelRange = visr::ChannelRange;
  using ChannelList = visr::ChannelList;
  //@}

  /**
   * Constructor.
   * @param context Reference to a signal flow context object providing basic runtime parameters as period length or sampling frequency.
   * @param name "the name of the component. Used to address the component inside other components and for status reporting.
   * @param parent Reference (pointer) to a parent component if the present object is part of a containing signal flow.
   * If \p nullptr is passed, this component is the top level.
   */
  explicit CompositeComponent( SignalFlowContext const & context,
                               char const * name,
                               CompositeComponent * parent = nullptr );

  /**
   * Destructor.
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

  /**
   * Register a connection between parameter ports (both real ports of contained components or external placeholder ports).
   * @param sendComponent The name of the component holding the send port (local name, not the 
   * fully qualified name). When specifiying an external port of a composite component, use an empty string or \p "this".
   * @param sendPort The local (not fully qualified) name of the send port. 
   * @param receiveComponent The name of the component holding the receive port (local name, not the
   * fully qualified name). When specifiying an external port of a composite component, use an empty string or \p "this".
   * @param receivePort The local (not fully qualified) name of the receive port.
   * @throw std::invalid_argument if a specified component or port does not exist.
   */
  void parameterConnection( char const * sendComponent,
                            char const * sendPort,
                            char const * receiveComponent,
                            char const * receivePort );

  /**
   * Register a connection between parameter ports (both real ports of contained components or external placeholder ports).
   * @param sender Reference to the sendig port (retrieved, for example using \p Component::parameterPort() )
   * @param receiver Reference to the sendig port (retrieved, for example using \p Component::parameterPort() )
   */
   void parameterConnection( ParameterPortBase & sender,
                             ParameterPortBase & receiver );

  /**
   * Register an audio connection between a sending and a receiving audio port.
   * This overload uses C strings to denote both the names of the
   * components holding the ports and the output ports itself.
   * Lists of channel indices are to be specified for the sending and
   * the receiving port. The sizes of these lists must be identical,
   * and the contained indices must not exceed the width of the send
   * and receive port, respectively.
   * Empty lists for both the send and receive indices are permitted
   * and result in no connection.
   * @param sendComponent Name of the component holding the sending audio port. If the send port is an external input of this component, use "" or "this"
   * @param sendPort The name of the sending port.
   * @param sendIndices A list of channel indices denoting the send channels of the sending side.
   * @param receiveComponent Name of the component holding the receiving audio port. If the receive port is an external 
   * output of the present component, use "" or "this"
   * @param receivePort The name of the receiving port.
   * @param receiveIndices A list of channel indices denoting the receive channels within the receiver port.
   * @throw std::invalid_argument if a specified component or port does not exist.
   * @see ChannelList for the syntax to specify the channel index lists.
   * @deprecated The overloads using audio ports should be preferred over this variant using component names.
   */
  void audioConnection( char const * sendComponent,
                        char const * sendPort,
                        ChannelList const & sendIndices,
                        char const * receiveComponent,
                        char const * receivePort,
                        ChannelList const & receiveIndices );

  /**
  * Register an audio connection between a sending and a receiving audio port.
  * This overload uses audio ports (either directly referencing external in- and output of 
  * this components or retrieving ports of contained components using the Component::audioPort() method).
  * Lists of channel indices are to be specified for the sending and
  * the receiving port. The sizes of these lists must be identical,
  * and the contained indices must not exceed the width of the send
  * and receive port, respectively.
  * Empty lists for both the send and receive indices are permitted
  * and result in no connection.
  * @param sendPort The send port object.
  * @param sendIndices A list of channel indices denoting the send channels of the sending side.
  * @param receivePort The receive port object.
  * @param receiveIndices A list of channel indices denoting the receive channels within the receiver port.
  * @see ChannelList for the syntax to specify the channel index lists.
  */
  void audioConnection( AudioPortBase & sendPort,
                        ChannelList const & sendIndices,
                        AudioPortBase & receivePort,
                        ChannelList const & receiveIndices );

  /**
  * Register an audio connection between all channels of a sending and a receiving audio port.
  * This overload uses audio ports (either directly referencing external in- and output of
  * this components or retrieving ports of contained components using the Component::audioPort() method).
  * It establishes one-to-one connections between the channels of the sender and the receiver.
  * @param sendPort The send port object.
  * @param receivePort The receive port object.
  * @throw std::invalid_argument if the port widths do not match.
  */
  void audioConnection( AudioPortBase & sendPort,
                        AudioPortBase & receivePort );
};

} // namespace visr

#endif // #ifndef VISR_COMPOSITE_COMPONENT_HPP_INCLUDED
