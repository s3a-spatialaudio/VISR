/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_COMPOSITE_COMPONENT_HPP_INCLUDED
#define VISR_COMPOSITE_COMPONENT_HPP_INCLUDED

#include "component.hpp"
#include "channel_list.hpp"

#include <memory>
#include <vector>

namespace visr
{

// Forward declarations
class SignalFlowContext;

namespace impl
{
class CompositeComponent;
}

/**
 *
 *
 */
class CompositeComponent: public Component
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
   * Query whether this component is composite.
   * @return true
   */
  virtual bool isComposite() const override;

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
  impl::CompositeComponent & implementation();

  /**
  * Return a reference to the internal data structures holding ports and contained components, const version.
  * From the user point of view, these data structure is opaque and unknown.
  * @todo Improve name ('implementation' does not really fit)
  */
  impl::CompositeComponent const & implementation() const;

// protected:

  /**
   * TODO: Should we add an explicit addComponent() method instead of the implicit registration?
   */
  
  /**
   * Register a connection between parameter ports (both real ports of contained components or external placeholder ports.

   * @throw std::invalid_argument if a specified component or port does not exist.
   */
  void registerParameterConnection( std::string const & sendComponent,
                                    std::string const & sendPort,
                                    std::string const & receiveComponent,
                                    std::string const & receivePort );

  void registerParameterConnection( ParameterPortBase & sender,
                                    ParameterPortBase & receiver );


  void registerAudioConnection( std::string const & sendComponent,
                                std::string const & sendPort,
                                ChannelList const & sendIndices,
                                std::string const & receiveComponent,
                                std::string const & receivePort,
                                ChannelList const & receiveIndices );

  void registerAudioConnection( AudioPortBase & sendPort,
                                ChannelList const & sendIndices,
                                AudioPortBase & receivePort,
                                ChannelList const & receiveIndices );

  void registerAudioConnection( AudioPortBase & sendPort,
                                AudioPortBase & receivePort );

private:
  std::unique_ptr<impl::CompositeComponent> mImpl;
};

} // namespace visr

#endif // #ifndef VISR_COMPOSITE_COMPONENT_HPP_INCLUDED
