/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "initialise_parameter_library.hpp"

#include "double_buffering_protocol.hpp"
#include "message_queue_protocol.hpp"
#include "shared_data_protocol.hpp"

#include "biquad_parameter.hpp"
#include "filter_routing_parameter.hpp"
#include "indexed_value_parameter.hpp"
#include "matrix_parameter.hpp"
#include "listener_position.hpp"
#include "interpolation_parameter.hpp"
#include "object_vector.hpp"
#include "scalar_parameter.hpp"
#include "string_parameter.hpp"
#include "signal_routing_parameter.hpp"
#include "time_frequency_parameter.hpp"
#include "vector_parameter.hpp"

#include <libvisr/parameter_factory.hpp>
#include <libvisr/communication_protocol_factory.hpp>

#include <vector>

namespace visr
{
namespace pml
{

/**
 * Helper template class to register and unregister parameter types 
 * following the RAII idiom.
 * By creating a variable of a specific template type, an arbitrary number of parameter types can be registered.
 * They are automatically unregistered when the lifetime of the variable ends.
 * To this end, the template accepts an arbitary number of parameter types as template parameters.
 *
 */
//@{
/**
 * Base case of the recursive variadic template.
 * An empty registrar object (with no associated parameter types)
 */
template< class ... Parameter >
class ParameterRegistrar
{
public:
  /**
   * Constructor (trival).
   */
  ParameterRegistrar() = default;
  /**
   * Destructor (trival)
   */
  ~ParameterRegistrar() {}
};

/**
 * Variadic macro, acepting one or more paramter types as template parameters.
 */
template< class Parameter, class ... Parameters >
class ParameterRegistrar< Parameter, Parameters ...>: public ParameterRegistrar< Parameters ... >
{
public:
  /**
   * Constructor, registers the first element of the template argument list and passes the remaining elements 
   * recursively to a another ParameterRegistrar template, which forms the base class of the this class.
   */
  ParameterRegistrar()
    : ParameterRegistrar<Parameters...>()
  {
    visr::ParameterFactory::registerParameterType<Parameter>();
  }

  /**
   * Destructors, performs unregistration of the first parameter type in the 
   * template argument list, while the remaining elements are unregistered 
   * recursively.
   */
  ~ParameterRegistrar()
  {
    visr::ParameterFactory::unregisterParameterType<Parameter>();
  }
};
//@}

/**
 * Helper template class to register and unregister protocol types
 * following the RAII idiom.
 * By creating a variable of a specific template type, an arbitrary number of protocol types can be registered.
 * They are automatically unregistered when the lifetime of the variable ends.
 * To this end, the template accepts an arbitary number of protocol types as template arguments.
 *
 */
 //@{
 /**
  * Base case of the recursive variadic template.
  * An empty registrar object (with no associated protocol types)
  */
template< class ... Protocol >
class ProtocolRegistrar
{
public:
  /**
   * Constructor (trival).
   */
  ProtocolRegistrar() = default;
  /**
   * Destructor (trival)
   */
  ~ProtocolRegistrar() {}
};

/**
 * Variadic macro, acepting one or more paramter types as template protocols.
 */
template< class Protocol, class ... Protocols >
class ProtocolRegistrar< Protocol, Protocols ...> : public ProtocolRegistrar< Protocols ... >
{
public:
  /**
   * Constructor, registers the first element of the template argument list and passes the remaining elements
   * recursively to a another protocolRegistrar template, which forms the base class of the this class.
   */
  ProtocolRegistrar()
    : ProtocolRegistrar<Protocols...>()
  {
    visr::CommunicationProtocolFactory::registerCommunicationProtocol<Protocol>();
  }

  /**
   * Destructors, performs unregistration of the first parameter type in the
   * template argument list, while the remaining elements are unregistered
   * recursively.
   */
  ~ProtocolRegistrar()
  {
    visr::CommunicationProtocolFactory::unregisterCommunicationProtocol<Protocol>();
  }
};
//@}


void initialiseParameterLibrary()
{
  static ParameterRegistrar<
    BiquadParameterMatrix<float>,
    BiquadParameterMatrix<double>,
    FilterRoutingParameter,
    FilterRoutingListParameter,
    IndexedVectorDoubleType,
    IndexedVectorFloatType,
    IndexedStringType,
    InterpolationParameter,
    ListenerPosition,
    MatrixParameter<float>,
    MatrixParameter<double>,
    MatrixParameter<std::complex<float> >,
    MatrixParameter<std::complex<double> >,

    ObjectVector,

    ListenerPosition,

    ScalarParameter<bool>,
    ScalarParameter<int>,
    ScalarParameter<unsigned int>,
    ScalarParameter<float>,
    ScalarParameter<double>,
    ScalarParameter<std::complex<float> >,
    ScalarParameter<std::complex<double> >,

    SignalRoutingParameter,

    StringParameter,

    TimeFrequencyParameter<float>,
    TimeFrequencyParameter<double>,

    VectorParameter<float>,
    VectorParameter<double> >
 sParameterRegistrar;

  static ProtocolRegistrar<
    DoubleBufferingProtocol,
    MessageQueueProtocol,
    SharedDataProtocol >
  sProtocolRegistrar;


}

} // namespace pml
} // namespace visr
