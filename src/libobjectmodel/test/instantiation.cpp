/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/audio_object.hpp>
#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/point_source.hpp>

#include <iostream>
#include <cstdlib>

int main( int argc, char** argv )
{
  using namespace visr::objectmodel;

  PointSource foo( 1 );

  ObjectVector scene;

  scene.set( 1, foo );

  return EXIT_SUCCESS;
}
