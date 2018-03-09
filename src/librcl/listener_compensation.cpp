//
//  LoudspeakerArray.h
//
//  Created by Marcos F. Sim�n G�lvez on 02/02/2015.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//

// avoid annoying warning about unsafe STL functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

#include "listener_compensation.hpp"

#include <cstdio>

#include <libpanning/defs.h>
#include <libpanning/XYZ.h>

#include <libpml/vector_parameter_config.hpp>

#include <boost/filesystem.hpp>

#include <algorithm>
#include <cmath>

// Uncomment to get debug output
// #define DEBUG_LISTENER_COMPENSATION 1

#ifdef DEBUG_LISTENER_COMPENSATION
#include <iostream>
#include <iterator>
#include <algorithm>
#endif

// TODO: Move definition to a separate file.
#define c_0 344


namespace visr
{
namespace rcl
{
  ListenerCompensation::ListenerCompensation( SignalFlowContext const & context,
                                              char const * name,
                                              CompositeComponent * parent,
                                              panning::LoudspeakerArray const & arrayConfig )
  : AtomicComponent( context, name, parent )
  , m_listenerPos( 0.0f, 0.0f, 0.0f )
  , mNumberOfLoudspeakers( arrayConfig.getNumRegularSpeakers() )
  , mPositionInput( "positionInput", *this, pml::EmptyParameterConfig() )
  , mGainOutput( "gainOutput", *this )
  , mDelayOutput( "delayOutput", *this)
{
  m_array = arrayConfig;
  pml::VectorParameterConfig const vectorConfig( mNumberOfLoudspeakers );

  mGainOutput.setParameterConfig( vectorConfig );
  mDelayOutput.setParameterConfig( vectorConfig );
}

void ListenerCompensation::process()
{
  if( mPositionInput.changed() )
  {
    pml::ListenerPosition const & pos( mPositionInput.data());
    efl::BasicVector<SampleType> & gains( mGainOutput.data());
    efl::BasicVector<SampleType> & delays( mDelayOutput.data());

    if (gains.size() != mNumberOfLoudspeakers or delays.size() != mNumberOfLoudspeakers)
    {
      throw std::invalid_argument("ListenerCompensation::process(): The size of the gain or delay vector does not match the number of loudspeaker channels.");
    }

    setListenerPosition(pos.x(), pos.y(), pos.z());
    if( calcGainComp( gains ) != 0 )
    {
      throw std::runtime_error("ListenerCompensation::process(): calcGainComp() failed.");
    }
    if (calcDelayComp( delays ) != 0)
    {
      throw std::runtime_error("ListenerCompensation::process(): calcDelayComp() failed.");
    }

#ifdef DEBUG_LISTENER_COMPENSATION
    std::cout << "DelayVector Source Gain: ";
    std::copy( gains.data(), gains.data()+gains.size(), std::ostream_iterator<float>(std::cout, " ") );
    std::cout << "Delay [s]: ";
    std::copy( delays.data(), delays.data()+delays.size(), std::ostream_iterator<float>(std::cout, " ") );
    std::cout << std::endl;
#endif // DEBUG_LISTENER_COMPENSATION

    mPositionInput.resetChanged();
    mGainOutput.swapBuffers();
    mDelayOutput.swapBuffers();
  }
}

int ListenerCompensation::calcGainComp( efl::BasicVector<Afloat> & gainComp )
{
  panning::XYZ l1;
  Afloat rad = 0.0f, max_rad = 0.0f, x = 0.0f, y = 0.0f, z = 0.0f;

  //setting listener position
  x = m_listenerPos.x;
  y = m_listenerPos.y;
  z = m_listenerPos.z;

  for (std::size_t i = 0; i < m_array.getNumSpeakers(); i++) {

    l1 = m_array.getPosition( i );

    rad = std::sqrt(std::pow((l1.x - x), 2.0f) + std::pow((l1.y - y), 2.0f) + std::pow((l1.z - z), 2.0f));

    gainComp[i] = rad;

    if ( gainComp[i]>max_rad)
      max_rad = gainComp[i];
  }

  for (std::size_t i = 0; i < m_array.getNumSpeakers(); i++) {

    gainComp[i] = (gainComp[i]/max_rad);
  }
  return 0;
}


int ListenerCompensation::calcDelayComp( efl::BasicVector<Afloat> & delayComp )
{
  
  Afloat rad=0.0f, max_rad=0.0f, x=0.0f, y=0.0f, z=0.0f;
  panning::XYZ l1;

  //setting listener position
  x = m_listenerPos.x;
  y = m_listenerPos.y;
  z = m_listenerPos.z;

  for (std::size_t i = 0; i < m_array.getNumSpeakers(); i++) {

    l1 = m_array.getPosition( i );

    rad = std::sqrt(std::pow((l1.x-x),2.0f) + std::pow((l1.y-y),2.0f) + std::pow((l1.z-z),2.0f));

    delayComp[i] = rad;

    if (delayComp[i]>max_rad)
      max_rad = delayComp[i];
  }

  for ( std::size_t i = 0; i < m_array.getNumSpeakers(); i++){

    delayComp[i] = std::abs(delayComp[i]-max_rad)/c_0;
  }
  return 0;
}

}// rcl
}//visr

