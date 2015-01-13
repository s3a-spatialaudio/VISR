/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "scene_decoder.hpp"

#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/object_vector_parser.hpp>

#include <libpml/message_queue.hpp>

#include <iostream>

namespace visr
{
namespace rcl
{

SceneDecoder::SceneDecoder( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
{
}

SceneDecoder::~SceneDecoder()
{
}

void SceneDecoder::setup( )
{
}

void SceneDecoder::process( pml::MessageQueue<std::string> & messages, objectmodel::ObjectVector & objects )
{
  while( !messages.empty() )
  {
    std::string const & nextMsg = messages.nextElement();
    try
    {
      objectmodel::ObjectVectorParser::updateObjectVector( nextMsg, objects );
    }
    catch( std::exception const & ex )
    {
      std::cerr << "SceneDecoder: Error while decoding a scene metadata message: " << ex.what() << std::endl;
    }
    messages.popNextElement();
  }
}

} // namespace rcl
} // namespace visr
