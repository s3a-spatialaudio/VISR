/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/signal_routing.hpp>

#include <libril/audio_signal_flow.hpp>
#include <libpml/signal_routing_parameter.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <string>

namespace visr
{
namespace rcl
{
namespace test
{

namespace // unnamed
{
class RoutingSimple: public ril::AudioSignalFlow
{
public:
  RoutingSimple()
    : ril::AudioSignalFlow( 1024, 48000 )
    , mRouter( *this, "Router" )
  {
  }

  void setup()
  {
    pml::SignalRoutingParameter initRouting{ { 0, 1 }, { 2, 0 }, { 5, 7 } };
    mRouter.setup( 6, 8, initRouting );
  }

  void process()
  {
    mRouter.process();
  }

private:
  rcl::SignalRouting mRouter;
};

} // namespace unnamed

BOOST_AUTO_TEST_CASE( InstantiateSignalRoutingParameter )
{
  pml::SignalRoutingParameter::Entry defaultEntry;
  pml::SignalRoutingParameter::Entry explEntry = { 0, 0 };
  pml::SignalRoutingParameter::Entry copyEntry( explEntry );

  pml::SignalRoutingParameter::Entry initEntry{ 2, 3 };

  pml::SignalRoutingParameter const emptyParameter;
  pml::SignalRoutingParameter const initParameter( { { 4, 3 }, { 4, 7 }, { 3, 8 } } );
  pml::SignalRoutingParameter const initParameter2{ { 0, 0 }, pml::SignalRoutingParameter::Entry{ 2, 3 }, pml::SignalRoutingParameter::Entry{ 4, 7 } };

  pml::SignalRoutingParameter copyParameter = initParameter;
}

BOOST_AUTO_TEST_CASE( InstantiateSignalRouting )
{
  RoutingSimple mFlow;

  mFlow.setup();

}

BOOST_AUTO_TEST_CASE( parseSignalRoutingParameter )
{
  std::string const param1Encoded( "4=3, 4=7  , 3=8" );
  pml::SignalRoutingParameter toFill1;
  bool success = toFill1.parse( param1Encoded );
  pml::SignalRoutingParameter const initParameterCheck( { { 4, 3 }, { 4, 7 }, { 3, 8 } } );
}

} // namespace test
} // namespace rcl
} // namespce visr
