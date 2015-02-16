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

#include <libpanning/defs.h>
#include <libpanning/XYZ.h>
#include <libpanning/LoudspeakerArray.h>

namespace visr
{

	namespace rcl
	{

		class ListenerCompensation
		{
		private:
			LoudspeakerArray* m_array; //passing the address of the loudspeaker array
			XYZ m_listenerPos; //position of the listener
			Afloat m_GainComp[MAX_NUM_SPEAKERS]; // stores the compensation for the Gain
			Afloat m_DelayComp[MAX_NUM_SPEAKERS];// stores the compensation for the Delay


		public:

			int setLoudspeakerArray(LoudspeakerArray* array){ // assigning the geometry
				m_array = array;
				return 0;
			}


			int getNumSpeakers() {
				return m_array->m_nSpeakers;
			}

			int setListenerPosition(Afloat x, Afloat y, Afloat z){ //assigning the position of the listener
				m_listenerPos.set(x, y, z);
				return 0;
			}

			int calcGainComp(); // this function calculates the gain compensation
			Afloat(*getGains())[MAX_NUM_SPEAKERS] {

				return &m_GainComp;
			}

			int calcDelayComp(); // this function calculates the delay compensation
			Afloat(*getDelays())[MAX_NUM_SPEAKERS] {

				return &m_DelayComp;

			}


		};//class Listener Compensation


	} // namespace rcl
} // namespace visr


#endif /* defined(__S3A_renderer_dsp__LoudspeakerArray__) */
