//
//  LoudspeakerArray.h
//
//  Created by Marcos F. Simón Gálvez on 02/02/2015.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//

// avoid annoying warning about unsafe STL functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

#include "listener_compensation.hpp"
#include <cmath>
#include <algorithm>

#include <cstdio>

#include <libpanning/defs.h>
#include <libpanning/XYZ.h>

#include <boost/filesystem.hpp>

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
ListenerCompensation::ListenerCompensation(ril::AudioSignalFlow& container, char const * name)//constructor
  : ril::AudioComponent(container, name)
  , m_listenerPos( 0.0f, 0.0f, 0.0f )
  , mNumberOfLoudspeakers( 0 )
{
}

void ListenerCompensation::setup(std::size_t numberOfLoudspeakers, std::string const & arrayConfig)
{
  mNumberOfLoudspeakers = numberOfLoudspeakers;

  boost::filesystem::path const filePath = absolute(boost::filesystem::path(arrayConfig));

  FILE* rawHandle = fopen(filePath.string().c_str(), "r");
  if (rawHandle == 0)
  {
    throw std::invalid_argument(std::string("ListenerCompensation::setup(): Cannot open loudspeaker configuration file: ") + strerror(errno) + ".");
  }
  if (m_array.load(rawHandle) != 0)
  {
    fclose(rawHandle);
    throw std::invalid_argument("ListenerCompensation::setup(): Error parsing loudspeaker configuration file.");
  }
  fclose(rawHandle);

  if (mNumberOfLoudspeakers != static_cast<std::size_t>(m_array.getNumSpeakers()))
  {
    throw std::invalid_argument("ListenerCompensation::setup() The loudspeaker configuration file does not match to the given number of loudspeaker channels.");
  }

}

void ListenerCompensation::process(pml::ListenerPosition const & pos,
                                   efl::BasicVector<SampleType> & gains, efl::BasicVector<SampleType> & delays)
{
  if (gains.size() != mNumberOfLoudspeakers or delays.size() != mNumberOfLoudspeakers)
  {
    throw std::invalid_argument("ListenerCompensation::process(): The size of the gain or delay vector does not match the number of loudspeaker channels.");
  }
  setListenerPosition(pos.x(), pos.y(), pos.z());
  if( calcGainComp() != 0 )
  {
    throw std::runtime_error("ListenerCompensation::process(): calcGainComp() failed.");
  }
  if (calcDelayComp() != 0)
  {
    throw std::runtime_error("ListenerCompensation::process(): calcDelayComp() failed.");
  }

//		gains = m_GainComp; //copying the values from delays
//		delays = m_DelayComp;
  std::copy(&m_GainComp[0], &m_GainComp[0] + mNumberOfLoudspeakers, gains.data());
  std::copy(&m_DelayComp[0], &m_DelayComp[0] + mNumberOfLoudspeakers, delays.data());

#ifdef DEBUG_LISTENER_COMPENSATION
  std::cout << "DelayVector Source Gain: ";
  std::copy( gains.data(), gains.data()+gains.size(), std::ostream_iterator<float>(std::cout, " ") );
  std::cout << "Delay [s]: ";
  std::copy( delays.data(), delays.data()+delays.size(), std::ostream_iterator<float>(std::cout, " ") );
  std::cout << std::endl;
#endif // DEBUG_LISTENER_COMPENSATION
}

int ListenerCompensation::calcGainComp()
{
  int i;
  panning::XYZ l1;
  Afloat rad = 0.0f, max_rad = 0.0f, x = 0.0f, y = 0.0f, z = 0.0f;

  //setting listener position
  x = m_listenerPos.x;
  y = m_listenerPos.y;
  z = m_listenerPos.z;

  for (i = 0; i < m_array.getNumSpeakers(); i++) {

    l1 = m_array.m_position[i];

    rad = std::sqrt(std::pow((l1.x - x), 2.0f) + std::pow((l1.y - y), 2.0f) + std::pow((l1.z - z), 2.0f));

    m_GainComp[i] = rad;

    if (m_GainComp[i]>max_rad)
      max_rad = m_GainComp[i];
  }

  for (i = 0; i < m_array.m_nSpeakers; i++) {

    m_GainComp[i] = (m_GainComp[i]/max_rad);
  }
  return 0;

}


int ListenerCompensation::calcDelayComp()
{
  int i;
  Afloat rad=0.0f, max_rad=0.0f, x=0.0f, y=0.0f, z=0.0f;
  panning::XYZ l1;


  //setting listener position
  x = m_listenerPos.x;
  y = m_listenerPos.y;
  z = m_listenerPos.z;


  for (i = 0; i < m_array.m_nSpeakers; i++) {

    l1 = m_array.m_position[i];

			
    rad = std::sqrt(std::pow((l1.x-x),2.0f) + std::pow((l1.y-y),2.0f) + std::pow((l1.z-z),2.0f));

    m_DelayComp[i] = rad;

    if (m_DelayComp[i]>max_rad)
      max_rad = m_DelayComp[i];
  }

  for (i = 0; i < m_array.m_nSpeakers; i++) {

    m_DelayComp[i] = std::abs(m_DelayComp[i]-max_rad)/c_0;
  }
  return 0;
}

}// rcl
}//visr

