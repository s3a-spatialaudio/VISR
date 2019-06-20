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

class ParameterRegistrarBase
{
};

template< typename T >
class ParameterRegistrar: public ParameterRegistrarBase
{
public:
  ParameterRegistrar()
  {
    ParameterFactory::registerParameterType< T >();
  }

  ParameterRegistrar(ParameterRegistrar const &) = delete;

  ~ParameterRegistrar()
  {
    ParameterFactory::unregisterParameterType< T >();
  }
};

class ProtocolRegistrarBase
{
};

template< typename T >
class ProtocolRegistrar : public ProtocolRegistrarBase
{
public:
  ProtocolRegistrar()
  {
    CommunicationProtocolFactory::registerCommunicationProtocol< T >();
  }

  ~ProtocolRegistrar()
  {
    CommunicationProtocolFactory::unregisterCommunicationProtocol< T >();
  }
};


class InitObject
{
public:
  InitObject()
  {

  }

  ~InitObject()
  {

  }
private:
  std::vector<ParameterRegistrarBase> mParameters = {
  ParameterRegistrar< BiquadParameterMatrix<double> >(),
  ParameterRegistrar< FilterRoutingParameter >(),
  ParameterRegistrar< FilterRoutingListParameter >(),

  ParameterRegistrar< IndexedVectorDoubleType >(),
  ParameterRegistrar< IndexedVectorFloatType >(),
  ParameterRegistrar< IndexedStringType >(),

  ParameterRegistrar< InterpolationParameter >(),

  ParameterRegistrar< ListenerPosition >(),

  ParameterRegistrar< MatrixParameter<float> >(),
  ParameterRegistrar< MatrixParameter<double> >(),
  ParameterRegistrar< MatrixParameter<std::complex<float> > >(),
  ParameterRegistrar< MatrixParameter<std::complex<double> > >(),

  ParameterRegistrar< ObjectVector >(),

  ParameterRegistrar< ListenerPosition >(),

  ParameterRegistrar< ScalarParameter<bool> >(),
  ParameterRegistrar< ScalarParameter<int> >(),
  ParameterRegistrar< ScalarParameter<unsigned int> >(),
  ParameterRegistrar< ScalarParameter<float> >(),
  ParameterRegistrar< ScalarParameter<double> >(),
  ParameterRegistrar< ScalarParameter<std::complex<float> > >(),
  ParameterRegistrar< ScalarParameter<std::complex<double> > >(),

  ParameterRegistrar< SignalRoutingParameter >(),

  ParameterRegistrar< StringParameter >(),

  ParameterRegistrar< TimeFrequencyParameter<float> >(),
  ParameterRegistrar< TimeFrequencyParameter<double> >(),

  ParameterRegistrar< VectorParameter<float> >(),
  ParameterRegistrar< VectorParameter<double> >(),
  };

  std::vector<ProtocolRegistrarBase> mProtocols =
  {
    ProtocolRegistrar< DoubleBufferingProtocol >(),
    ProtocolRegistrar< MessageQueueProtocol >(),
    ProtocolRegistrar< SharedDataProtocol >()
  };
};


void initialiseParameterLibrary()
{
  static InitObject sInitLibrary;

  //CommunicationProtocolFactory::registerCommunicationProtocol< DoubleBufferingProtocol >();
  //CommunicationProtocolFactory::registerCommunicationProtocol< MessageQueueProtocol >();
  //CommunicationProtocolFactory::registerCommunicationProtocol< SharedDataProtocol >();

  //// Register all supported parameter types.
  //ParameterFactory::registerParameterType< BiquadParameterMatrix<float> >( BiquadParameterMatrix<float>::staticType() );
  //ParameterFactory::registerParameterType< BiquadParameterMatrix<double> >( BiquadParameterMatrix<double>::staticType() );

  //ParameterFactory::registerParameterType< FilterRoutingParameter >( FilterRoutingParameter::staticType() );
  //ParameterFactory::registerParameterType< FilterRoutingListParameter >( FilterRoutingListParameter::staticType() );

  //ParameterFactory::registerParameterType< IndexedVectorDoubleType >( IndexedVectorDoubleType::staticType() );
  //ParameterFactory::registerParameterType< IndexedVectorFloatType >( IndexedVectorFloatType::staticType() );
  //ParameterFactory::registerParameterType< IndexedStringType >( IndexedStringType::staticType() );

  //ParameterFactory::registerParameterType< InterpolationParameter >( InterpolationParameter::staticType() );

  //ParameterFactory::registerParameterType< ListenerPosition >( ListenerPosition::staticType() );

  //ParameterFactory::registerParameterType< MatrixParameter<float> >( MatrixParameter<float>::staticType() );
  //ParameterFactory::registerParameterType< MatrixParameter<double> >( MatrixParameter<double>::staticType() );
  //ParameterFactory::registerParameterType< MatrixParameter<std::complex<float> > >( MatrixParameter<std::complex<float> >::staticType() );
  //ParameterFactory::registerParameterType< MatrixParameter<std::complex<double> > >( MatrixParameter<std::complex<double> >::staticType() );

  //ParameterFactory::registerParameterType< ObjectVector >(ObjectVector::staticType() );

  //ParameterFactory::registerParameterType< ListenerPosition >( );

  //ParameterFactory::registerParameterType< ScalarParameter<bool> >();
  //ParameterFactory::registerParameterType< ScalarParameter<int> >();
  //ParameterFactory::registerParameterType< ScalarParameter<unsigned int> >();
  //ParameterFactory::registerParameterType< ScalarParameter<float> >();
  //ParameterFactory::registerParameterType< ScalarParameter<double> >();
  //ParameterFactory::registerParameterType< ScalarParameter<std::complex<float> > >();
  //ParameterFactory::registerParameterType< ScalarParameter<std::complex<double> > >();

  //ParameterFactory::registerParameterType< SignalRoutingParameter >();

  //ParameterFactory::registerParameterType< StringParameter >();

  //ParameterFactory::registerParameterType< TimeFrequencyParameter<float> >();
  //ParameterFactory::registerParameterType< TimeFrequencyParameter<double> >();

  //ParameterFactory::registerParameterType< VectorParameter<float> >();
  //ParameterFactory::registerParameterType< VectorParameter<double> >();

//  initialised = true;
}

} // namespace pml
} // namespace visr
