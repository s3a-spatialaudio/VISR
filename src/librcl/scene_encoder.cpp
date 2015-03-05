/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "scene_encoder.hpp"

#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/object_vector_parser.hpp>

#include <libpml/message_queue.hpp>

#include <iostream>

namespace visr
{
namespace rcl
{

SceneEncoder::SceneEncoder( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
{
}

SceneEncoder::~SceneEncoder()
{
}

void SceneEncoder::setup( )
{
}

void SceneEncoder::process( objectmodel::ObjectVector const & objects, pml::MessageQueue<std::string> & messages )
{
  std::stringstream msg;
  objectmodel::ObjectVectorParser::encodeObjectVector( objects, msg );
  messages.enqueue( msg.str() );
}

} // namespace rcl
} // namespace visr
