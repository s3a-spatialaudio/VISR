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

class PanningGainCalculator
{
public:
  explicit PanningGainCalculator( std::string arrayConfig);

  boost::python::list calculateGains(float x, float y, float z);

  std::size_t numberOfLoudspeakers() const
  {
	return mVbapCalculator.getNumSpeakers();
  }

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
    mArray.loadXml(configPath.string());
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

	std::copy(gains.row(0), gains.row(0) + gains.numberOfColumns(), std::ostream_iterator<Afloat>(std::cout, ", "));
	std::cout << std::endl;

	boost::python::list retVec;
	for (std::size_t elIdx(0); elIdx < gains.numberOfColumns(); ++elIdx)
	{
      retVec.append(gains.at(0,elIdx));
	}

	return retVec;
}
 
} // namespace namespace pythonbindings
} // namespace visr

// NOTE: The python module name must match the file name.
BOOST_PYTHON_MODULE( panning_gain_calculator )
{
  using namespace boost::python;
  using namespace visr::pythonbindings;

  class_<visr::pythonbindings::PanningGainCalculator, boost::noncopyable>("PanningGainCalculator", init<std::string>())
	  .def("calculateGains", &PanningGainCalculator::calculateGains)
	  .def("numberOfLoudspeakers", &PanningGainCalculator::numberOfLoudspeakers)
      .def("numberOfSources", &PanningGainCalculator::numberOfSources);
}
