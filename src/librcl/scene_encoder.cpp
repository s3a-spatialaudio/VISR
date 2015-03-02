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
#if 0
  while( !messages.empty() )
  {
    std::string const & nextMsg = messages.nextElement();
    try
    {
      objectmodel::ObjectVectorParser::updateObjectVector( nextMsg, objects );
    }
    catch( std::exception const & ex )
    {
      std::cerr << "SceneEncoder: Error while decoding a scene metadata message: " << ex.what() << std::endl;
    }
    messages.popNextElement();
  }
#endif
}

} // namespace rcl
} // namespace visr
