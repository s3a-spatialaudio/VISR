/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "switch_decoder.hpp"

#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/object_vector_parser.hpp>

#include <libpml/empty_parameter_config.hpp>

#include <oscpkt.hh>

#include <iostream>

namespace visr
{
namespace rcl
{

SwitchDecoder::SwitchDecoder( SignalFlowContext& context,
                              char const * name,
                              CompositeComponent * parent /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mDatagramInput( "datagramInput", *this, pml::StringParameterConfig( 255 ) )
 , mRoutingOutput( "routingOutput", *this, pml::EmptyParameterConfig() )
 , mOscParser( new oscpkt::PacketReader() )
{
}

SwitchDecoder::~SwitchDecoder()
{
}

void SwitchDecoder::setup(std::size_t numInputs, std::size_t numOutputs, std::size_t initialInput)
{
  mNumberOfInputs = numInputs;
  mNumberOfOutputs = numOutputs;
  mInitialInputIndex = initialInput;
}

void SwitchDecoder::process()
{
  // TODO: Use "routing" to generate the output message.
  // pml::SignalRoutingParameter & routing = mRoutingOutput.data();
  constexpr int cInvalidIdx = std::numeric_limits<int>::max();
  int newIndex = cInvalidIdx;
  // Do we need to set a routing on the first process() call?
  while (not mDatagramInput.empty())
  {
    std::string const & nextMsg = mDatagramInput.front();
    try
    {
      mOscParser->init(nextMsg.data(), nextMsg.size());

      oscpkt::Message* msg;
      while ((msg = mOscParser->popMessage()) != nullptr)
      {
        int idx;
        if (msg->arg().popInt32(idx))
        {
          newIndex = idx;
        }
        else
        {
          std::cerr << "SwitchDecoder: Failure while decoding OSC message." << std::endl;
          // Skip invalid packets.
        }
      }
    }
    catch (std::exception const & ex)
    {
      std::cerr << "SceneDecoder: Error decoding an OSC scene metadata message: " << ex.what() << std::endl;
    }
    mDatagramInput.pop();
  }
  // Use the latest received index.
  if( newIndex != cInvalidIdx )
  {
    if( (newIndex < 0) or (static_cast<std::size_t>(newIndex) >= mNumberOfInputs))
    {
      std::cerr << "SwitchDecoder: Received index exceeds number of outputs." << std::endl;
    }
    else
    {
      // TODO: Create a signal routing and send it.
      pml::SignalRoutingParameter & routing = mRoutingOutput.data();

      routing.clear();
      for (std::size_t outputIndex(0); outputIndex < mNumberOfOutputs; ++outputIndex)
      {
        // We need a way to distinguish between the different input channels.
        routing.addRouting(outputIndex, outputIndex);
      }

      mRoutingOutput.swapBuffers();
    }
  }
}

} // namespace rcl
} // namespace visr
