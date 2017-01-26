/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_COMPOSITE_COMPONENT_HPP_INCLUDED
#define VISR_LIBRIL_COMPOSITE_COMPONENT_HPP_INCLUDED

#include <libril/component.hpp>

// Do we need the types for that (or would forward declarations suffice)?
// TODO: Move this to the private implementation object.
// !! At the moment this is a hack because of the circular dependency!!
#include <libvisr_impl/audio_connection_descriptor.hpp>
#include <libvisr_impl/parameter_connection_descriptor.hpp>

#include <map>
#include <memory>

namespace visr
{
namespace ril
{
// Forward declarations
class SignalFlowContext;

class CompositeComponentImplementation;

/**
 *
 *
 */
class CompositeComponent: public Component
{
public:

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
  CompositeComponentImplementation & implementation();

  /**
  * Return a reference to the internal data structures holding ports and contained components, const version.
  * From the user point of view, these data structure is opaque and unknown.
  * @todo Improve name ('implementation' does not really fit)
  */
  CompositeComponentImplementation const & implementation() const;

protected:

  void registerParameterConnection( std::string const & sendComponent,
                                    std::string const & sendPort,
                                    std::string const & receiveComponent,
                                    std::string const & receivePort );

  void registerAudioConnection( std::string const & sendComponent,
                                std::string const & sendPort,
                                AudioChannelIndexVector const & sendIndices,
                                std::string const & receiveComponent,
                                std::string const & receivePort,
                                AudioChannelIndexVector const & receiveIndices );

  void registerAudioConnection( Component const & sendComponent,
                                std::string const & sendPort,
                                AudioChannelIndexVector const & sendIndices,
                                Component const & receiveComponent,
                                std::string const & receivePort,
                                AudioChannelIndexVector const & receiveIndices );

  void registerAudioConnection( Component const & sendComponent,
                                std::string const & sendPort,
                                Component const & receiveComponent,
                                std::string const & receivePort );


  void registerAudioConnection( AudioPort & sender,
                                AudioChannelIndexVector const & sendIndices,
                                AudioPort & receiver,
                                AudioChannelIndexVector const & receiveIndices );

private:
  std::unique_ptr<CompositeComponentImplementation> mImpl;
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_COMPOSITE_COMPONENT_HPP_INCLUDED
