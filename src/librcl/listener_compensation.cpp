//
//  LoudspeakerArray.h
//
//  Created by Marcos F. Simón Gálvez on 02/02/2015.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//

#include "listener_compensation.hpp"
#include <math.h>
#include <algorithm>    // std::min_element, std::max_element

//#include <iostream>
#include <stdio.h>

#include <libpanning/defs.h>
#include <libpanning/XYZ.h>


#define c_0 344


namespace visr
{
	namespace rcl
	{

		int ListenerCompensation::calcGainComp()
		{
			int i;
			XYZ l1;
			Afloat rad=0.0f, x=0.0f, y=0.0f, z=0.0f;

			//setting listener position
			x -= m_listenerPos.x;
			y -= m_listenerPos.y;
			z -= m_listenerPos.z;

			for (i = 0; i < m_array->m_nSpeakers; i++) {

				l1 = m_array->m_position[m_array->m_triplet[i][0]];//obtaining coordinates from loudspeaker i

				rad = sqrt(pow((l1.x - x), 2.0) + pow((l1.y - y), 2.0) + pow((l1.z - z), 2.0));
				
				m_GainComp[i] = rad;

			}
			return 0;

		}


		int ListenerCompensation::calcDelayComp()
		{
			int i;
			Afloat rad=0.0f, max_rad=0.0f, x=0.0f, y=0.0f, z=0.0f;
			XYZ l1;


				//setting listener position
				x -= m_listenerPos.x;
				y -= m_listenerPos.y;
				z -= m_listenerPos.z;


				for (i = 0; i < m_array->m_nSpeakers; i++) {

				l1 = m_array->m_position[m_array->m_triplet[i][0]];//obtaining coordinates from loudspeaker i
			
				rad = sqrt(pow((l1.x-x),2.0) + pow((l1.y-y),2.0) + pow((l1.z-z),2.0));

				m_DelayComp[i] = rad;

				if (m_DelayComp[i]>max_rad)
					max_rad = m_DelayComp[i];
				}

				for (i = 0; i < m_array->m_nSpeakers; i++) {

				m_DelayComp[i] = abs(m_DelayComp[i]-max_rad)/c_0*44100;
				}

				return 0;
		}

	}// rcl
}//visr

