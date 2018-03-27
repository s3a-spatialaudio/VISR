/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libvisr/audio_port_base.hpp>
#include <libvisr/channel_list.hpp> 

#include <libvisr/impl/audio_connection_descriptor.hpp>
#include <libvisr/impl/audio_port_base_implementation.hpp>
#include <libvisr/impl/component_implementation.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <cassert>
#include <iterator>
#include <numeric>
#include <ostream>
#include <sstream>

namespace visr
{
namespace python
{
namespace visr
{

namespace // unnamed
{

/**
  * Local definition of an output stream operator.
  * TODO: Decide whether to move that to ChannelList
  */
std::ostream & operator<<( std::ostream & str, ChannelList const channels )
{
  if( channels.size() == 0 ) // Avoid problems with invalid iterators etc.
  {
    return str;
  }
// Trivial implementation
#if 1
  std::copy( channels.begin(), channels.end(), std::ostream_iterator<ChannelList::IndexType>(str, ", ") );
#else
  using DiffVec = std::vector<std::ptrdiff_t>;
  DiffVec differences;
  differences.reserve( channels.size() - 1 );
  std::adjacent_difference(channels.begin(), channels.end(), std::back_inserter(differences),
    []( ChannelList::IndexType lhs, ChannelList::IndexType rhs ){ return rhs - lhs; } ); // Make sure the operation uses signed ints
  DiffVec::iterator startIt = differences.begin();
  while( startIt != differences.end() )
  {
    DiffVec::iterator nextDiff = std::adjacent_find( startIt, differences.end(), []( DiffVec::value_type a, DiffVec::value_type b ){ return a != b; } );
    std::ptrdiff_t seqLen = nextDiff - startIt;
    assert( seqLen > 0 );
    std::size_t startIdx( static_cast<std::size_t>(startIt - differences.begin()) );
    std::size_t endIdx( static_cast<std::size_t>(nextDiff - differences.begin()) );
    if( seqLen == 1 )
    {
      str << channels[startIdx];
    }
    else if( seqLen == 2 )
    {
      str << channels[startIdx] << "," << channels[startIdx+1];
    }
    else
    {
      ChannelList::IndexType const lastIdx = channels[endIdx-1];
      if( differences[startIdx] == 1 )
      {
        str << channels[startIdx] << ":" << lastIdx;
      }
      else
      {
        str << channels[startIdx] << ":" << differences[startIdx] << ":" << lastIdx;
      }
    }
    if( nextDiff != differences.end() )
    {
      str <<",";
    }
    startIt = nextDiff;
  }
#endif
  return str;
}

} // unnamed namespace

void exportAudioConnection( pybind11::module& m )
{
  pybind11::class_<impl::AudioConnection>(m, "AudioConnection" )
   .def( pybind11::init<>(), "Default constructor" )
   .def( pybind11::init( [](AudioPortBase & sendPort, ChannelList const & sendIndices, AudioPortBase & receivePort, ChannelList const & receiveIndices )
   {
     return new impl::AudioConnection( &(sendPort.implementation()), sendIndices, &(receivePort.implementation()), receiveIndices );
   }), pybind11::arg("sendPort"), pybind11::arg("sendIndices"), pybind11::arg("receivePort"), pybind11::arg("receiveIndices"), "Full constructor (usually not needed)." )
   .def( "__lt__", &impl::AudioConnection::operator<, pybind11::arg("rhs"), "Comparison operator (for ordering connections." )
   .def_property_readonly( "sender", []( impl::AudioConnection const & self ){ return self.sender()->containingPort(); }, pybind11::return_value_policy::reference )
   .def_property_readonly( "receiver", []( impl::AudioConnection const & self ) { return self.receiver()->containingPort(); } , pybind11::return_value_policy::reference )
   .def_property_readonly( "sendIndices", &impl::AudioConnection::sendIndices )
   .def_property_readonly( "receiveIndices", &impl::AudioConnection::receiveIndices )
   .def( "__str__", []( impl::AudioConnection const & conn )
   {
     std::stringstream repr;
     repr << conn.sender()->parent().name() << ":" << conn.sender()->name() << ":" << conn.sendIndices() 
          << conn.receiver()->parent().name() << ":" << conn.receiver()->name() << ":" << conn.receiveIndices();
   })
    ;
}

} // namepace visr
} // namespace python
} // namespace visr

