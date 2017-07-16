/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_SCALAR_OSC_DECODER_HPP_INCLUDED
#define VISR_LIBRCL_SCALAR_OSC_DECODER_HPP_INCLUDED

#include <libril/atomic_component.hpp>

#include <libril/parameter_input.hpp>
#include <libril/parameter_output.hpp>

#include <libpml/string_parameter.hpp>
#include <libpml/message_queue_protocol.hpp>
#include <libpml/scalar_parameter.hpp>

#include <memory> // for std::unique_ptr
#include <vector>

// Forward declarations
namespace oscpkt
{
  class PacketReader;
}

namespace visr
{

namespace rcl
{

/**
 * Component to decode OSC messages containing a single integer and to fill a signal routing parameter list accordingly.
 */
class ScalarOscDecoder: public AtomicComponent
{
public:
  enum class DataType
  {
    Boolean,
    Integer,
    UnsignedInteger,
    Float,
    Double
    // complex types don't make sense here.
  };

  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component.
   */
  explicit ScalarOscDecoder( SignalFlowContext const & context,
                         char const * name,
                         CompositeComponent * parent = nullptr );

  /**
   * Disabled (deleted) copy constructor
   */
  ScalarOscDecoder( ScalarOscDecoder const & ) = delete;


  /**
   * Destructor.
   */
  ~ScalarOscDecoder();

  /**
   * Method to initialise the component.
   */ 
  void setup( char const * dataType );

  /**
   * The process function. 
   */
  void process();

private:
  ParameterInput< pml::MessageQueueProtocol, pml::StringParameter > mDatagramInput;

  std::unique_ptr<oscpkt::PacketReader> mOscParser;

  std::unique_ptr<ParameterOutput< pml::MessageQueueProtocol, pml::ScalarParameter<bool> > > mBoolOutput;
  std::unique_ptr<ParameterOutput< pml::MessageQueueProtocol, pml::ScalarParameter<int> > > mIntOutput;
  std::unique_ptr<ParameterOutput< pml::MessageQueueProtocol, pml::ScalarParameter<unsigned int> > > mUIntOutput;
  std::unique_ptr<ParameterOutput< pml::MessageQueueProtocol, pml::ScalarParameter<float> > > mFloatOutput;
  std::unique_ptr<ParameterOutput< pml::MessageQueueProtocol, pml::ScalarParameter<double> > > mDoubleOutput;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_SCALAR_OSC_DECODER_HPP_INCLUDED
