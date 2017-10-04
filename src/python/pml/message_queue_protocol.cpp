/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/message_queue_protocol.hpp>

#include <libvisr/communication_protocol_base.hpp>

#include <pybind11/pybind11.h>

namespace visr
{

using pml::MessageQueueProtocol;

namespace python
{
namespace pml
{

void exportMessageQueueProtocol( pybind11::module & m)
{
  pybind11::class_<MessageQueueProtocol, visr::CommunicationProtocolBase>
    messagequeue( m, "MessageQueueProtocol" );

  messagequeue
    .def_property_readonly_static( "staticName", [](pybind11::object /*self*/){ return MessageQueueProtocol::staticName(); } )
    .def_property_readonly_static( "staticType", []( pybind11::object /*self*/ ){ return MessageQueueProtocol::staticType(); } )
    .def( pybind11::init<ParameterType const &, ParameterConfigBase const & >() )
    ;

  pybind11::class_<MessageQueueProtocol::InputBase, CommunicationProtocolBase::Input>( messagequeue, "InputBase" )
    .def( pybind11::init<>() )
    .def( "empty", &MessageQueueProtocol::InputBase::empty, "Query whether the queue is empty." )
    .def( "size", &MessageQueueProtocol::InputBase::size, "Return the number of elements in the queue" )
    .def( "front", &MessageQueueProtocol::InputBase::front, pybind11::return_value_policy::reference, "Return a reference to the next element in the queue, throw an exception if the queue is empty." )
    .def( "pop", &MessageQueueProtocol::InputBase::pop, "Clear the front-most element from the queue. If the queue is empty, an exception is thrown." )
    .def( "clear", &MessageQueueProtocol::InputBase::clear, "Clear all elements from the queue" )
    ;

  pybind11::class_<MessageQueueProtocol::OutputBase, CommunicationProtocolBase::Output>( messagequeue, "OutputBase" )
    .def( pybind11::init<>() )
    .def( "empty", &MessageQueueProtocol::OutputBase::empty, "Query whether the queue is empty." )
    .def( "size", &MessageQueueProtocol::OutputBase::size, "Return the number of elements in the queue" )
    .def( "enqueue", [](MessageQueueProtocol::OutputBase& self, const ParameterBase & param ){ self.enqueue(param.clone()); }, "Place the element the back of the queue" )
    ;
}

} // namepace pml
} // namespace python
} // namespace visr
