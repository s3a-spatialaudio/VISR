/* Copyright Institue of Sound and Vibration Research - All rights reserved. */

#include <librrl/audio_signal_flow.hpp>
#include <librrl/integrity_checking.hpp>

#include <libril/atomic_component.hpp>
#include <libril/parameter_input_port.hpp>
#include <libril/parameter_output_port.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#include <libpml/double_buffering_protocol.hpp>
#include <libpml/matrix_parameter.hpp>
#include <libpml/matrix_parameter_config.hpp>
#include <libpml/initialise_parameter_library.hpp>

#include <boost/test/unit_test.hpp>

#include <ciso646>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

namespace visr
{
namespace rrl
{
namespace test
{

namespace // unnamed
{

std::size_t const numRows =3;
std::size_t const numCols = 5;

class Sender: public AtomicComponent
{
public:
  Sender( SignalFlowContext & context, char const * componentName, CompositeComponent * parent )
   : AtomicComponent( context, componentName, parent )
   , mParamOutput( "paramOut", *this, pml::MatrixParameterConfig(numRows, numCols) )
  {
  }
  void process() override {}
private:
  ParameterOutputPort<pml::DoubleBufferingProtocol, pml::MatrixParameter<float> > mParamOutput;
};

class Receiver: public AtomicComponent
{
public:
  Receiver( SignalFlowContext & context, char const * componentName, CompositeComponent * parent )
    : AtomicComponent( context, componentName, parent )
    , mParamInput( "paramIn", *this, pml::MatrixParameterConfig( numRows, numCols ) )
  {
  }
  void process() override
  {
    pml::MatrixParameter<float> const & mtx = mParamInput.data();
    if( (mtx.numberOfRows() != numRows) or (mtx.numberOfColumns() != numCols) )
    {
      status( StatusMessage::Warning, "ParameterConnection::Receiver: Matrix dimension mismatch. Expected (", numRows, ",", numCols,
        ") received (", mtx.numberOfRows(), ",", mtx.numberOfColumns(), ")." );
    }
  
  }
private:
  ParameterInputPort<pml::DoubleBufferingProtocol, pml::MatrixParameter<float> > mParamInput;
};

class MyComposite: public CompositeComponent
{
public:
  MyComposite( SignalFlowContext & context, char const * componentName, CompositeComponent * parent )
    : CompositeComponent( context, componentName, parent )
    , mSender( context, "Sender", this )
    , mReceiver( context, "Receiver", this)
  {
    registerParameterConnection( mSender.parameterPort("paramOut"), mReceiver.parameterPort("paramIn") );
  }
private:
  Sender mSender;
  Receiver mReceiver;
};

} // namespace unnamed

BOOST_AUTO_TEST_CASE( CheckParamConnection )
{
  pml::initialiseParameterLibrary();

  const std::size_t blockSize{64};
  std::size_t const numBlocks{16};

  SignalFlowContext context( blockSize, 48000 );

  MyComposite myComp( context, "topLevel", nullptr );

  std::stringstream checkMsg;
  bool const res = rrl::checkConnectionIntegrity( myComp, true, checkMsg );
  BOOST_CHECK_MESSAGE( res, checkMsg.str().c_str() );

  rrl::AudioSignalFlow flow( myComp );

  float ** inputPtr =  nullptr;
  float ** outputPtr = nullptr;

  for( std::size_t blockIdx(0); blockIdx < numBlocks; ++blockIdx )
  {
    flow.process( inputPtr, outputPtr );
  }



}

} // namespace test
} // namespace rrl
} // namespace visr
