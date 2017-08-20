/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/signal_routing_parameter.hpp>
#include <libpml/empty_parameter_config.hpp> 

#include <libril/constants.hpp>
#include <libril/parameter_base.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace visr
{

using pml::SignalRoutingParameter;
//using pml::SignalRoutingParameter::IndexType;

namespace python
{
namespace pml
{

namespace py = pybind11;

void exportSignalRoutingParameter( py::module & m)
{
  py::class_<SignalRoutingParameter> sigParam( m, "SignalRoutingParameter");

  py::class_<SignalRoutingParameter::Entry>( sigParam, "Entry" )
    .def( "__init__", []( SignalRoutingParameter::Entry & inst, std::size_t in, std::size_t out )
          { new (&inst) SignalRoutingParameter::Entry{ in, out }; } )
    .def_readwrite( "input", &SignalRoutingParameter::Entry::input )
    .def_readwrite( "output", &SignalRoutingParameter::Entry::output )
  ;

  sigParam
   .def_poperty_readonly("size", &SignalRoutingParameter::size )
   .def( py::init<>() )
   .def( py::init<std::initializer_list<SignalRoutingParameter::Entry> const &>(), py::arg("initList"))
////   .def( "__iter__", [](SignalRoutingParameter& sr){ return py::make_iterator(sr.begin(), sr.end()); }, "Return a Python iterator over the contained routings." )
   .def( "swap", &SignalRoutingParameter::swap )
//   .def_poperty_readonly("empty", &SignalRoutingParameter::empty )
   .def( "addRouting", static_cast<void(SignalRoutingParameter::*)(SignalRoutingParameter::IndexType, SignalRoutingParameter::IndexType)>(&SignalRoutingParameter::addRouting),
         py::arg("input"), py::arg("output") )
//#if 0
//  /**
//   * Add a routing entry for the (input, output) pair contained in the entry.
//   * An existing routing entry for the output index is deleted.
//   */
//  void addRouting( Entry const & newEntry );
//
//  /**
//   * Remove a routing entry consisting of a input and an output index.
//   * @return If a routing for this (input, output) pair existed before, false if not.
//   */
//  bool removeEntry( Entry const & entry );
//
//  /**
//   * Remove a routing for a given output index.
//   * @return True if there was a routing for that output, false if no such entry existed.
//   */
//  bool removeEntry( IndexType outputIdx );
//
//  /**
//   * Clear all routing entries.
//   */
//#endif
//         .def( "clear", , &SignalRoutingParameter::clear );
//         .def( "getEntry", &SignalRoutingParameter::getEntry );
//         .def( "getInput", &SignalRoutingParameter::getIn;
//         .def( "getOutput", &SignalRoutingParameter::getOutput;
//         .def( "parse", &SignalRoutingParameter::parse, py::arg("encoded")  );
  ;
 ;
#if 0
  void swap( SignalRoutingParameter& rhs );

  bool empty() const { return mRoutings.empty(); }

  std::size_t size() const { return mRoutings.size(); }

  SignalRoutingParameter & operator=(SignalRoutingParameter const & rhs) = default;

  RoutingsType::const_iterator begin() const { return mRoutings.begin(); }

  RoutingsType::const_iterator end() const { return mRoutings.end(); }

  void addRouting( IndexType inputIdx, IndexType outputIdx )
  {
    addRouting( Entry{ inputIdx, outputIdx } );
  }

  /**
   * Add a routing entry for the (input, output) pair contained in the entry.
   * An existing routing entry for the output index is deleted.
   */
  void addRouting( Entry const & newEntry );

  /**
   * Remove a routing entry consisting of a input and an output index.
   * @return If a routing for this (input, output) pair existed before, false if not.
   */
  bool removeEntry( Entry const & entry );

  /**
   * Remove a routing for a given output index.
   * @return True if there was a routing for that output, false if no such entry existed.
   */
  bool removeEntry( IndexType outputIdx );

  /**
   * Clear all routing entries.
   */
  void clear();

  Entry const & getEntry( IndexType outputIdx ) const
  {
    static const Entry returnInvalid{ cInvalidIndex, cInvalidIndex };

    RoutingsType::const_iterator const findIt = mRoutings.find( Entry{ cInvalidIndex, outputIdx } );
    return findIt == mRoutings.end() ? returnInvalid : *findIt;
  }

  IndexType getInput( IndexType outputIdx ) const
  {
    RoutingsType::const_iterator const findIt = mRoutings.find( Entry{ cInvalidIndex, outputIdx } );
    return findIt == mRoutings.end() ? cInvalidIndex : findIt->input;
  }

  IndexType getOutput( IndexType inputIdx ) const;

  bool parse( std::string const & encoded );
#endif
  ;
}

} // namepace pml
} // namespace python
} // namespace visr
