/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/hoa_allrad_gain_calculator.hpp>

#include <libril/signal_flow_context.hpp>

#include <libobjectmodel/object_vector.hpp>

#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <string>

namespace visr
{

// forward declaration
namespace panning
{
class LoudspeakerArray;
}
namespace pml
{
template<typename T>
class MatrixParameter;
}

namespace rcl
{
namespace test
{

using namespace objectmodel;

#if 0
namespace // unnamed
{
  class HoaCalc: public ril::AudioSignalFlow
  {
  public:
    HoaCalc( )
      : ril::AudioSignalFlow( 1024, 48000 )
      , mHoaCalc( *this, "HoaCalc" )
    {
    }

    void process( )
    {
      mHoaCalc.process( mObjVec, mPanningGains );
    }

    void setObjectVector( objectmodel::ObjectId id, objectmodel::Object const & obj );

  private:
    rcl::HoaAllRadGainCalculator mHoaCalc;

    objectmodel::ObjectVector mObjVec;

    efl::BasicMatrix<float> mPanningGains;

  };
}
#endif

BOOST_AUTO_TEST_CASE( InstantiateAllRad )
{
//  HoaCalc mFlow;
}

BOOST_AUTO_TEST_CASE( HoaAllradGainCalculatorSetup )
{
}

} // namespace test
} // namespace rcl
} // namespce visr
