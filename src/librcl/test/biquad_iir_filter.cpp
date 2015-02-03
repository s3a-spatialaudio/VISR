/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/biquad_iir_filter.hpp>

#include <libril/audio_signal_flow.hpp>
#include <libpml/biquad_parameter.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <string>

namespace visr
{
namespace rcl
{
namespace test
{

namespace // unnamed
{
class BiquadSimple: public ril::AudioSignalFlow
{
public:
  BiquadSimple()
    : ril::AudioSignalFlow( 1024, 48000 )
    , mBiquad( *this, "Filter" )
  {
  }

  void setup()
  {
  }

  void process()
  {
    mBiquad.process();
  }

private:
  rcl::BiquadIirFilter mBiquad;
};
}

BOOST_AUTO_TEST_CASE( InstantiateBiquad )
{
  BiquadSimple mFlow;
  // TODO: Implement me!
}

} // namespace test
} // namespace rcl
} // namespce visr
