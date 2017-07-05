/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "scene_decoder.hpp"

#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/object_vector_parser.hpp>

#include <libpml/empty_parameter_config.hpp>

#include <iostream>

namespace visr
{
namespace rcl
{

  SceneDecoder::SceneDecoder( SignalFlowContext const & context,
                              char const * name,
                              CompositeComponent * parent /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mDatagramInput( "datagramInput", *this, pml::EmptyParameterConfig() )
 , mObjectVectorOutput( "objectVectorOutput", *this, pml::EmptyParameterConfig() )
{
}

SceneDecoder::~SceneDecoder()
{
}

void SceneDecoder::setup( )
{
}

void SceneDecoder::process()
{
  objectmodel::ObjectVector & objects = mObjectVectorOutput.data();
  while( not mDatagramInput.empty() )
  {
    std::string const & nextMsg = mDatagramInput.front();
    try
    {
      objectmodel::ObjectVectorParser::updateObjectVector( nextMsg, objects );
      mObjectVectorOutput.swapBuffers();
    }
    catch( std::exception const & ex )
    {
      std::cerr << "SceneDecoder: Error while decoding a scene metadata message: " << ex.what() << std::endl;
    }
    mDatagramInput.pop();
  }
}

} // namespace rcl
} // namespace visr
