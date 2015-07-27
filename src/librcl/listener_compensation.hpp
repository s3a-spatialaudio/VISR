//
//  LoudspeakerArray.h
//
//  Created by Marcos F. Simón Gálvez on 02/02/2015.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//


#ifndef __S3A_renderer_dsp__listenerCompensation__

#define __S3A_renderer_dsp__listenerCompensation__

//#include <iostream>
#include <cstdio>

#include <libefl/basic_matrix.hpp>

#include <libril/audio_component.hpp>

#include <libpanning/defs.h>
#include <libpanning/XYZ.h>
#include <libpanning/LoudspeakerArray.h>

#include <libefl/basic_vector.hpp>

#include <libpml/listener_position.hpp>

namespace visr
{

namespace rcl
{

class ListenerCompensation: public ril::AudioComponent
{
public:
  using SampleType = ril::SampleType;
private:
  panning::LoudspeakerArray m_array; //passing the address of the loudspeaker array
  panning::XYZ m_listenerPos; //position of the listener
  std::size_t mNumberOfLoudspeakers;
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit ListenerCompensation(ril::AudioSignalFlow& container, char const * name);

  /**
   * Disabled (deleted) copy constructor
   */
  ListenerCompensation(ListenerCompensation const &) = delete;


  void setup( panning::LoudspeakerArray const & arrayConfig );

  /**
   * The process function.
   * It takes a listener position as input and calculates a gain vector and a delay vector.
   */
  void process(pml::ListenerPosition const & pos, efl::BasicVector<SampleType> & gains, efl::BasicVector<SampleType> & delays );


  std::size_t getNumSpeakers()  const {
    return m_array.getNumSpeakers();
  }

  int setListenerPosition(Afloat x, Afloat y, Afloat z){ //assigning the position of the listener
    m_listenerPos.set(x, y, z);
    return 0;
  }

  /**
   * Internal method to calculate the compensation gains.
   * @param [out] gainComp The result vector for the calculated gains (linear scale). It must have the dimension 'numberOfLoudspeakers'.
   * @return 0 in case of success.
   */
  int calcGainComp( efl::BasicVector<Afloat> & gainComp ); // this function calculates the gain compensation

  /**
  * Internal method to calculate the compensation delays.
  * @param [out] delayComp The result vector for the calculated delays (in seconds). It must have the dimension 'numberOfLoudspeakers'.
  * @return 0 in case of success.
  */
  int calcDelayComp( efl::BasicVector<Afloat> & delayComp ); // this function calculates the delay compensation

};//class Listener Compensation


} // namespace rcl
} // namespace visr


#endif /* defined(__S3A_renderer_dsp__LoudspeakerArray__) */
