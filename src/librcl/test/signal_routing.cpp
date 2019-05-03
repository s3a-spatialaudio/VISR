/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/signal_routing.hpp>

#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>
#include <libpml/signal_routing_parameter.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
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
class RoutingSimple: public CompositeComponent
{
public:
  RoutingSimple( SignalFlowContext const & context,
                 char const * name,
                 CompositeComponent * parent )
    : CompositeComponent( context, name, parent )
    , mRouter( context, "Router", this )
  {
  }

  void setup()
  {
    pml::SignalRoutingParameter initRouting{ { 0, 1 }, { 2, 0 }, { 5, 7 } };
    mRouter.setup( 6, 8, initRouting );
  }

private:
  rcl::SignalRouting mRouter;
};

} // namespace unnamed

BOOST_AUTO_TEST_CASE( InstantiateSignalRoutingParameter )
{
  pml::SignalRoutingParameter::Entry const explEntry = { 3, 7 };
  BOOST_CHECK( explEntry.input == 3 );
  BOOST_CHECK( explEntry.output == 7 );

  pml::SignalRoutingParameter::Entry copyEntry( explEntry );
  BOOST_CHECK( copyEntry.input == explEntry.input );
  BOOST_CHECK( copyEntry.output == explEntry.output );


  pml::SignalRoutingParameter const emptyParameter;
  BOOST_CHECK( emptyParameter.size() == 0 );

  pml::SignalRoutingParameter const initParameter( { { 4, 3 }, { 4, 7 }, { 3, 8 } } );

  BOOST_CHECK( initParameter.size() == 3 );
  pml::SignalRoutingParameter const initParameter2{ { 0, 0 }, pml::SignalRoutingParameter::Entry{ 2, 3 }, pml::SignalRoutingParameter::Entry{ 4, 7 } };

  pml::SignalRoutingParameter copyParameter = initParameter;
  BOOST_CHECK( copyParameter.size() == initParameter.size() );
}

BOOST_AUTO_TEST_CASE( InstantiateSignalRouting )
{
  SignalFlowContext const context{64,48000};
  RoutingSimple mFlow( context, "toplevel", nullptr/*no parent*/ );
  // TODO: Add checks!
}

BOOST_AUTO_TEST_CASE( parseSignalRoutingParameter )
{
  std::string const param1Encoded( "4=3, 4=7  , 3=8" );
  pml::SignalRoutingParameter toFill1;
  bool success = toFill1.parse( param1Encoded );
  BOOST_CHECK( success );
  pml::SignalRoutingParameter const initParameterCheck( { { 4, 3 }, { 4, 7 }, { 3, 8 } } );
}

} // namespace test
} // namespace rcl
} // namespce visr
