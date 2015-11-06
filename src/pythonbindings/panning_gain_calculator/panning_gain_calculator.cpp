/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <boost/python.hpp>

#include <iostream>

namespace visr
{
namespace pythonbindings
{

class PanningGainCalculator
{
};

void hello()
{
  std::cout << "Hello Python, this is C++ speaking." << std::endl;
}
 
} // namespace namespace pythonbindings
} // namespace visr

BOOST_PYTHON_MODULE( helloNS )
{
  using namespace boost::python;
  def( "hello", visr::pythonbindings::hello );
}
