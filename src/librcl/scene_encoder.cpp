/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "scene_encoder.hpp"

#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/object_vector_parser.hpp>

#include <libpml/empty_parameter_config.hpp>
#include <libpml/string_parameter_config.hpp>

#include <ciso646>
#include <sstream>

namespace visr
{
namespace rcl
{

SceneEncoder::SceneEncoder( ril::AudioSignalFlow& container, char const * name )
 : AtomicComponent( container, name )
 , mObjectInput( *this, "objectInput", pml::EmptyParameterConfig( ) )
 , mDatagramOutput( *this, "messageOutput", pml::StringParameterConfig( 32768 ) )
{
}

SceneEncoder::~SceneEncoder()
{
}

void SceneEncoder::setup( )
{
}

void SceneEncoder::process()
{
  std::stringstream msg;
  objectmodel::ObjectVectorParser::encodeObjectVector( mObjectInput.data(), msg );
  mDatagramOutput.enqueue( pml::StringParameter( msg.str( ) ) );
}

} // namespace rcl
} // namespace visr
