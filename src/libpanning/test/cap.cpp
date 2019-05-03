//
//  main.cpp
//
//  Created by Dylan Menzies on 10/11/2014.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//

#include <libpanning/VBAP.h>
#include <libpanning/CAP.h>
#include <libpanning/AllRAD.h>
#include <boost/filesystem.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace panning
{
namespace test
{

namespace // unnamed
{

void capTestCase( std::string const & arrayConfig,
                  XYZ const & sourcePos,
                  XYZ const & listenerPos, 
                  XYZ const & auralAxis,
                  std::vector<Afloat> const & referenceGains )
{
  LoudspeakerArray array;
  BOOST_CHECK_NO_THROW( array.loadXmlString( arrayConfig ) );

  CAP cap;
  cap.setLoudspeakerArray( &array );
  cap.setListenerPosition( listenerPos.x, listenerPos.y, listenerPos.z );
  cap.setListenerAuralAxis( auralAxis.x, auralAxis.y, auralAxis.z );     // head forward (rL left)

  cap.setNumSources( 1 );
  cap.setSourcePositions( &sourcePos );

  BOOST_CHECK( cap.calcGains() == 0);

  if( not referenceGains.empty() )
  {
    efl::BasicMatrix<Afloat> const & gains = cap.getGains();
    BOOST_CHECK_MESSAGE( gains.numberOfRows() == referenceGains.size(), "CAP gain vector size differs from reference." );

    std::size_t const numCheckEl = std::min( gains.numberOfRows(), referenceGains.size() );
    for( std::size_t idx(0); idx < numCheckEl; ++idx )
    {
      BOOST_CHECK_CLOSE( gains(idx,0), referenceGains[idx], 1.0e-4f );
    }
  }
}

/**
 * Overloaded version that can be used with initializer lists, that is,
 * constant arrays of the form {0.0, 1.0}
 */
void capTestCase( std::string const & arrayConfig,
  XYZ const & sourcePos,
  XYZ const & listenerPos,
  XYZ const & auralAxis,
  std::initializer_list<Afloat> const & referenceGains = {} )
{
  capTestCase( arrayConfig,
               sourcePos,
               listenerPos,
               auralAxis,
               std::vector<Afloat>(referenceGains.begin(), referenceGains.end() ));
}

// Define some setups to be used later on.
std::string const arrayTwoSpeakers =
"<panningConfiguration dimension = \"3\" infinite = \"false\">"
"<loudspeaker channel = \"1\" id =\"1\" > <cart x = \"1.0\" y = \"1.0\" z= \"0.0\" /></loudspeaker>"
"<loudspeaker channel = \"2\" id = \"2\" > <cart x = \"1.0\" y = \"-1.0\" z = \"0.0\" /></loudspeaker>"
"</panningConfiguration>";

std::string const arrayThreeSpeakers =
"<panningConfiguration dimension = \"3\" infinite = \"false\">"
"<loudspeaker channel = \"1\" id =\"1\" > <cart x = \"0.0\" y = \"1.0\" z= \"0.0\" /></loudspeaker>"
"<loudspeaker channel = \"2\" id = \"2\" > <cart x = \"0.0\" y = \"-1.0\" z = \"0.0\" /></loudspeaker>"
"<loudspeaker channel = \"3\" id = \"3\" > <cart x = \"1.0\" y = \"1.0\" z = \"0.0\" /></loudspeaker>"
"</panningConfiguration>";

} // Unnamed namespace 


BOOST_AUTO_TEST_CASE( Cap2Loudspeakers )
{
  XYZ const sourcePos(0.0f, 1.0f, 0.0f, true );
  XYZ const listenerPos( 0.0f, 0.0f, 0.0f );
  XYZ const auralAxis( 0.0f, 1.0f, 0.0f ); // head forward (rL left)

  // TODO: Add reference gains!
  capTestCase( arrayTwoSpeakers, sourcePos, listenerPos, auralAxis /*, {0.5f, 0.5f }*/ );
}

BOOST_AUTO_TEST_CASE( Cap3Loudspeakers )
{
  XYZ const sourcePos( 0.0f, 1.0f, 0.0f, true );     // image left
  XYZ const listenerPos( 0.0f, 0.0f, 0.0f );    // Listener
  XYZ const auralAxis( 0.0f, 1.0f, 0.0f );           // head forward (rL left)
  // XYZ const auralAxis( -1.0f, 0.0f, 0.0f );       // head left (rL back)

  // TODO: Add reference gains!
  capTestCase( arrayThreeSpeakers, sourcePos, listenerPos, auralAxis /*, { 0.5f, 0.5f, 0.5f }*/ );
}

} // namespace test
} // namespace panning
} // namespace visr
