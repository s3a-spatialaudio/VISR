/* Copyright Institute of Sound and Vibration Research - All rights reserved */

// Define macro to control the behaviour of boost::python in case the boost libraries are linked statically.
#if BOOST_USE_STATIC_LIBS
#define BOOST_PYTHON_STATIC_LIB
#endif

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#include "boost/python/extract.hpp"
#include "boost/python/numeric.hpp"


#include <libpanning/LoudspeakerArray.h>
#include <libpanning/VBAP.h>
#include <libpanning/XYZ.h>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace visr
{
namespace pythonbindings
{

/**
 * Simple wrapper class to expose the requireed functionality
 * of the VBAP object.
 */
class PanningGainCalculator
{
public:
  /**
   * Constructor.
   * @param arrayConfig File path to the loudspeaker configuration file in XML format.
   */
  explicit PanningGainCalculator( std::string arrayConfig);

  /**
   * Calculate the panning gains for the source position given Cartesian coordinates.
   * @param x X coordinate in meter.
   * @param y Y coordinate in meter.
   * @param z Z coordinate in meter.
   * @return Normalised panning gains as a Python list. The output is identical to the VBAP class, i.e., gains of imaginary speakers are returned as well.
   */
  boost::python::list calculateGains(float x, float y, float z);

  /**
   * Return the number of loudspeakers, including imaginary ones.
   */
  std::size_t numberOfLoudspeakers() const
  {
    return mVbapCalculator.getNumSpeakers();
  }

  /**
   * Return the number of source objects, which is constant (1)
   * at the time being.
   */
  std::size_t numberOfSources() const
  {
    return cNumberOfObjects;
  }
private:
  std::size_t cNumberOfObjects;
  
  mutable std::vector<panning::XYZ> mSourcePositions;
  
  panning::LoudspeakerArray mArray;
  
  panning::VBAP mVbapCalculator;
};

PanningGainCalculator::PanningGainCalculator(std::string arrayConfig)
  : cNumberOfObjects( 1 )
  , mSourcePositions( cNumberOfObjects )
{
  // std::cout << "PanningGainCalculator: constructor called." << std::endl;
  boost::filesystem::path const configPath = boost::filesystem::path(arrayConfig);
  if (!exists(configPath))
  {
    std::cout << "Path: " << configPath.string() << std::endl;
    throw std::invalid_argument( "The given array configuration path does not exist." );
  }
  try
  {
    mArray.loadXmlFile(configPath.string());
    mVbapCalculator.setLoudspeakerArray( &mArray );
    mVbapCalculator.setNumSources(cNumberOfObjects);
    mVbapCalculator.setSourcePositions(&mSourcePositions[0]);
    mVbapCalculator.calcInvMatrices();
  }
  catch (std::exception const & ex)
  {
    std::cout << "Error during construction: " << ex.what() << std::endl;
  }
}

boost::python::list PanningGainCalculator::calculateGains(float x, float y, float z)
{
  mSourcePositions[0] = visr::panning::XYZ(x, y, z);
  mVbapCalculator.calcGains();
  
  efl::BasicMatrix<Afloat> const & gains = mVbapCalculator.getGains();
# if 0
  std::copy(gains.row(0), gains.row(0) + gains.numberOfColumns(), std::ostream_iterator<Afloat>(std::cout, ", "));
  std::cout << std::endl;
#endif
  boost::python::list retVec;
  for (std::size_t elIdx(0); elIdx < gains.numberOfColumns(); ++elIdx)
  {
    retVec.append(gains.at(0,elIdx));
  }

  return retVec;
}
 
} // namespace namespace pythonbindings
} // namespace visr

/**
 * Python module declaration to expose the class and its interface to Python.
 * @note The python module name must match the file name.
 */
BOOST_PYTHON_MODULE( panning_gain_calculator )
{
  using namespace boost::python;
  using namespace visr::pythonbindings;

  class_<visr::pythonbindings::PanningGainCalculator, boost::noncopyable>("PanningGainCalculator", init<std::string>())
    .def("calculateGains", &PanningGainCalculator::calculateGains)
    .def("numberOfLoudspeakers", &PanningGainCalculator::numberOfLoudspeakers)
    .def("numberOfSources", &PanningGainCalculator::numberOfSources);
}
