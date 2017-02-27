/* Copyright Institue of Sound and Vibration Research - All rights reserved. */


#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#include <boost/test/unit_test.hpp>

#include <iostream>

namespace visr
{
namespace ril
{
namespace test
{

namespace // unnamed
{

class MyAtom: public AtomicComponent
{
public:
  MyAtom( SignalFlowContext & context, char const * componentName, CompositeComponent * parent )
    : AtomicComponent( context, componentName, parent )
  {
    std::cout << "MyAtom name: " << name( ) << ", full name: " << fullName( ) << std::endl;
  }

  void process() override {}
private:
};

class MyComposite: public CompositeComponent
{
public:
  MyComposite( SignalFlowContext & context, char const * componentName, CompositeComponent * parent )
   : CompositeComponent( context, componentName, parent )
   , mAtom( context, "SecondLevelAtom", this )
  {
    std::cout << "MyComposite name: " << name() << ", full name: " << fullName() << std::endl;
  }
private:
  MyAtom mAtom;
};

class MyTopLevel: public CompositeComponent
{
public:
  MyTopLevel( SignalFlowContext & context, char const * componentName, CompositeComponent * parent )
    : CompositeComponent( context, componentName, parent )
    , mComposite1( context, "FirstLevelComposite", this )
    , mAtomTopLevel( context, "FirstLevelAtom", this )
  {
    std::cout << "MyTopLevel name: " << name() << ", full name:" << fullName() << std::endl;
  }
private:
  MyComposite mComposite1;
  MyAtom mAtomTopLevel;
};

} // unnamed namespace

BOOST_AUTO_TEST_CASE( topLevelName )
{
  SignalFlowContext context( 1024, 48000 );
  MyTopLevel const flow( context, "", nullptr );
}

} // namespace test
} // namespace ril
} // namespace visr
