/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/scene_decoder.hpp>

#include <librcl/panning_gain_calculator.hpp>

#include <libril/communication_protocol_base.hpp>
#include <libril/communication_protocol_factory.hpp>
#include <libril/signal_flow_context.hpp>

#include <libvisr_impl/component_impl.hpp>
#include <libvisr_impl/parameter_port_base_implementation.hpp>

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
  SignalFlowContext context( 256, 48000 );

  rcl::SceneDecoder decoder( context, "decoder", nullptr );
  decoder.setup( );

  for( impl::ComponentImplementation::ParameterPortContainer::const_iterator paramPortIt = decoder.implementation().parameterPortBegin();
       paramPortIt != decoder.implementation().parameterPortEnd(); ++paramPortIt )
  {
    std::cout << "Found parameter port in scene decoder:" << (*paramPortIt)->name() << std::endl;

    ParameterType const paramType = (*paramPortIt)->parameterType();
    CommunicationProtocolType const protocolType = (*paramPortIt)->protocolType( );
    ParameterConfigBase const & paramConfig = (*paramPortIt)->parameterConfig();

    std::unique_ptr<CommunicationProtocolBase> protocol
      = CommunicationProtocolFactory::create( protocolType, paramType, paramConfig );
  }
}

} // namespace test
} // namespace rcl
} // namespce visr
