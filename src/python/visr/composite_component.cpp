/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "composite_component.hpp"

#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#ifdef USE_PYBIND11
#include <pybind11.h>
#else
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#include <boost/python/args.hpp>
#endif


#include <ciso646>
#include <iostream> // For debugging purposes only.


namespace visr
{
using ril::CompositeComponent;
namespace python
{
namespace visr
{

#ifdef USE_PYBIND11

/**
 * Wrapper class to get access to the full functionality
 * Apparently nor required anymore (and is troublesome when deducing the argument
 * type).
 */
class CompositeComponentWrapper: public CompositeComponent
{
public:
  /**
   * Use base class' constructors
   */
  using CompositeComponent::CompositeComponent;

  /**
   * Make protected methods available as public.
   */
  //@{
  using CompositeComponent::registerParameterConnection;
  using CompositeComponent::registerAudioConnection;
  //@}
};

void exportCompositeComponent( pybind11::module& m )
{
  /**
   * TODO: Decide whether we want additional inspection methods.
   * This would mean that we access the internal() object (probably adding methods to ComponentsWrapper)
   */
  pybind11::class_<ril::CompositeComponent, CompositeComponentWrapper, ril::Component >(m, "CompositeComponent" ) // Note: Trampoline class comes second.
    .def( pybind11::init<ril::SignalFlowContext &, char const*, CompositeComponent *>(),
          pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") = static_cast<CompositeComponent *>(nullptr) )
    // .def( pybind11::init<ril::SignalFlowContext &, char const*>(),
    // 	  pybind11::arg("context"), pybind11::arg("name") )
    .def_property_readonly( "numberOfComponents", &CompositeComponent::numberOfComponents )
    .def( "registerParameterConnection", &CompositeComponentWrapper::registerParameterConnection,
          pybind11::arg( "sendComponent"), pybind11::arg("sendPort"), pybind11::arg("receiveComponent"), pybind11::arg("receivePort") )
    // .def( "registerAudioConnection", &CompositeComponent::registerAudioConnection,
    // ( arg("sendComponent"), arg("sendPort"), arg("sendIndices"), arg("receiveComponent"), arg("receivePort"), arg("receiveIndices") ) )
    // TODO: Add further overloads of registerAudioConnection?
    ;
}

#else
using namespace boost::python;



/** Conversion facilities from Python types to visr::ril::AudioChannelIndexVector
 * @TODO Check whether we can put that into a namespace.
 */
//@{
struct AudioChannelIndexVectorFromPython
{
  /**
  *
  */
  AudioChannelIndexVectorFromPython()
  {
    converter::registry::push_back( &convertible, &construct, type_id<visr::ril::AudioChannelIndexVector>() );
    std::cout << "Registered AudioChannelIndexVectorFromPython converter." << std::endl;
  }

  /** Check whether the Python object can (possibly, not considering runtime errors)
   * be converted to a AudioChannelIndexVector 
   */
  static void * convertible( PyObject * pyObj )
  {
    // http://bazaar.launchpad.net/~yade-dev/yade/trunk/view/head:/py/wrapper/customConverters.cpp#L127
    // also checks for PyObject_HasAttrString(obj_ptr,"__len__")
    return PySequence_Check( pyObj ) ? pyObj : nullptr;
  }

  static void construct( PyObject* objPtr, boost::python::converter::rvalue_from_python_stage1_data* data )
  {
    using visr::ril::AudioChannelIndexVector;
    using ContainedType = AudioChannelIndexVector::IndexType;

    Py_ssize_t const len = PySequence_Size( objPtr );
    if( len < 0 )
    {
      throw std::invalid_argument( "Python to AudioChannelIndexVector conversion: invalid sequence size." );
    }
    std::size_t const length = static_cast<std::size_t>( len ); // That's safe after the check.
    // First construct the C++ sequence in  a stl vector
    std::vector<ContainedType> tmpVec(length);
    for( std::size_t idx(0); idx < length; ++idx )
    {
      tmpVec[idx] = extract<ContainedType>( PySequence_GetItem( objPtr, idx ) );
    }
    void* storage = (reinterpret_cast<converter::rvalue_from_python_storage<AudioChannelIndexVector>*>(data))->storage.bytes;
    new (storage) std::vector<AudioChannelIndexVector::IndexType>( tmpVec ); // Placement new from index vector
    data->convertible = storage;
  }
};
//@}

/**
 * Wrapper class to dispatch the virtual function call isComposite().
 * This seems to be a bit unnecessary because this is not going to be 
 * overridden by a Python class.
 */
class CompositeComponentWrapper: public CompositeComponent, public wrapper<CompositeComponent>
{
public:
  CompositeComponentWrapper( ril::SignalFlowContext & context,
                             char const * name,
                             ril::CompositeComponent * parent )
    : CompositeComponent( context, name,parent)
  {}

  using CompositeComponent::registerParameterConnection;

  using CompositeComponent::registerAudioConnection;
};


void exportCompositeComponent()
{
  /**
   * TODO: Decide whether we want additional inspection methods.
   * This would mean that we access the internal() object (probably adding methods to ComponentsWrapper)
   */
  class_<CompositeComponentWrapper, boost::noncopyable, bases<ril::Component> >("CompositeComponent", no_init )
    .def( init<ril::SignalFlowContext &, char const*, ril::CompositeComponent *>( 
      args("context", "name", "parent") ) )
    .add_property( "numberOfComponents", &CompositeComponent::numberOfComponents )
    .def( "registerParameterConnection", &CompositeComponent::registerParameterConnection,
          ( arg( "sendComponent"), arg("sendPort"), arg("receiveComponent"), arg("receivePort") ) )
    .def( "registerAudioConnection", &CompositeComponent::registerAudioConnection, 
    ( arg("sendComponent"), arg("sendPort"), arg("sendIndices"), arg("receiveComponent"), arg("receivePort"), arg("receiveIndices") ) )
    // TODO: Add further overloads of registerAudioConnection?
    ;
}
#endif

} // namepace visr
} // namespace python
} // namespace visr

