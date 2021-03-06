/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libvisr/atomic_component.hpp>
#include <libvisr/communication_protocol_base.hpp>
#include <libvisr/communication_protocol_factory.hpp>
#include <libvisr/polymorphic_parameter_input.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <libvisr/impl/component_implementation.hpp>

#include <libpml/initialise_parameter_library.hpp>
#include <libpml/double_buffering_protocol.hpp>
#include <libpml/matrix_parameter.hpp>
#include <libpml/matrix_parameter_config.hpp>

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>

namespace visr
{
namespace impl
{
namespace test
{

class TestAtom: public AtomicComponent
{
public:
  TestAtom( SignalFlowContext & context, char const * name,
            CompositeComponent * parent = nullptr )
   : AtomicComponent( context, name, parent )
    , mPolyInput( "polyIn", *this, pml::MatrixParameter<float>::staticType(),
      pml::DoubleBufferingProtocol::staticType(), pml::MatrixParameterConfig( 4,5 ) )
  {
    BOOST_CHECK( mPolyInput.protocolType() == pml::DoubleBufferingProtocol::staticType() );
  }

  void process() override
  {
  }
private:
  PolymorphicParameterInput mPolyInput;
};

BOOST_AUTO_TEST_CASE( ParameterConnection )
{
  pml::initialiseParameterLibrary();

  SignalFlowContext context( 64, 1024 );

  TestAtom( context, "PolyParameterTest" );
}

} // namespace test
} // namespace impl
} // namespce visr
