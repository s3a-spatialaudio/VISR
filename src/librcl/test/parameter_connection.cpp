/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/scene_decoder.hpp>

#include <librcl/panning_gain_calculator.hpp>

#include <libril/communication_protocol_base.hpp>
#include <libril/communication_protocol_factory.hpp>
#include <libril/signal_flow_context.hpp>


//#include <libpml/message_queue.hpp>
//#include <libpml/listener_position.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp> // case-insensitive string compare

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>

namespace visr
{
namespace rcl
{
namespace test
{

namespace // unnamed
{

} // unnamed namespace

BOOST_AUTO_TEST_CASE( ParameterConnection )
{
  ril::SignalFlowContext context( 256, 48000 );

  rcl::SceneDecoder decoder( context, "decoder", nullptr );
  decoder.setup( );

  for( ril::Component::ParameterPortContainer::const_iterator paramPortIt = decoder.parameterPortBegin();
    paramPortIt != decoder.parameterPortEnd(); ++paramPortIt )
  {
    std::cout << "Found parameter port in scene decoder:" << paramPortIt->first << std::endl;

    ril::ParameterType const paramType = paramPortIt->second->parameterType();
    ril::CommunicationProtocolType const protocolType = paramPortIt->second->protocolType( );
    ril::ParameterConfigBase const & paramConfig = paramPortIt->second->parameterConfig();

    std::unique_ptr<ril::CommunicationProtocolBase> protocol
      = ril::CommunicationProtocolFactory::create( protocolType, paramType, paramConfig );
  }
}

} // namespace test
} // namespace rcl
} // namespce visr
