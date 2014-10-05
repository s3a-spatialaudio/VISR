/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libefl/vector_functions.hpp>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>


int main( int argc, char const * const * argv )
{
  using namespace visr;

  std::cout << "Hello world. When I'm grown up, I wanna be a VBAP renderer" << std::endl;

  std::vector<float> a({ 1.0f, 1.0f, 2.0f, 3.0f, 4.0f });
  std::vector<float> b(a.size());

  efl::vectorAddConstant(3.0f, &a[0], &b[0], a.size(), 0);

  return EXIT_SUCCESS;
}
