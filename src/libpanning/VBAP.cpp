//
//  VBAP.cpp
//
//  Created by Dylan Menzies on 10/11/2014.
//  Copyright (c) ISVR, University of Southampton. All rights reserved.
//

// avoid annoying warning about unsafe STL functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

#include "VBAP.h"

#include <limits>

int VBAP::calcInvMatrices(){
    XYZ l1, l2, l3;
    Afloat det, temp;
    Afloat* inv;
    int i;
 
#ifdef VBAP_DEBUG_MESSAGES
    printf("calcInvMatrices()\n");   //!
#endif
        
    for (i = 0 ; i < m_array->m_nTriplets; i++) {
    
        if (m_array->m_triplet[i][0] == -1) continue;  //  triplet unused
            
	    if (m_array->m_triplet[i][0] == -1) continue; 

            l1 = m_array->m_position[m_array->m_triplet[i][0]];
            l2 = m_array->m_position[m_array->m_triplet[i][1]];
            if (m_array->is2D()) { l3.x = 0; l3.y = 0; l3.z = 1; }  // adapt 3D calc for 2D array
            else l3 = m_array->m_position[m_array->m_triplet[i][2]];
        
            // calc speaker positions relative to listener.
            l1.x = l1.x - m_listenerPos.x;
            l1.y = l1.y - m_listenerPos.y;
            l1.z = l1.z - m_listenerPos.z;
            l2.x = l2.x - m_listenerPos.x;
            l2.y = l2.y - m_listenerPos.y;
            l2.z = l2.z - m_listenerPos.z;
            l3.x = l3.x - m_listenerPos.x;
            l3.y = l3.y - m_listenerPos.y;
            l3.z = l3.z - m_listenerPos.z;

            l1.normalise();
            l2.normalise();
            l3.normalise();
        
            temp = (  l1.x * ((l2.y * l3.z) - (l2.z * l3.y))
                    - l1.y * ((l2.x * l3.z) - (l2.z * l3.x))
                    + l1.z * ((l2.x * l3.y) - (l2.y * l3.x)) );

            if (temp != 0.0f) det = 1.0f / temp;
            else det = 1.0f;
            
            inv = m_invMatrix[i];
            inv[0] = ((l2.y * l3.z) - (l2.z * l3.y)) * det;
            inv[3] = ((l1.y * l3.z) - (l1.z * l3.y)) * -det;
            inv[6] = ((l1.y * l2.z) - (l1.z * l2.y)) * det;
            inv[1] = ((l2.x * l3.z) - (l2.z * l3.x)) * -det;
            inv[4] = ((l1.x * l3.z) - (l1.z * l3.x)) * det;
            inv[7] = ((l1.x * l2.z) - (l1.z * l2.x)) * -det;
            inv[2] = ((l2.x * l3.y) - (l2.y * l3.x)) * det;
            inv[5] = ((l1.x * l3.y) - (l1.y * l3.x)) * -det;
            inv[8] = ((l1.x * l2.y) - (l1.y * l2.x)) * det;
    }

    return 0;
}




int VBAP::calcGains(){
    
    int i,j,jmin,l1,l2,l3;
    Afloat *inv;
    Afloat g1,g2,g3,g,gmin,g1min,g2min,g3min,x,y,z;

    
    //! Slow triplet search
    // Find triplet with highest minimum-gain-in-triplet (may be negative)
    
    jmin = -1; // indicate currently no triplet candidate.
#ifdef VBAP_DEBUG_MESSAGES
    printf("setListenerPosition %f %f %f\n",m_listenerPos.x,m_listenerPos.y,m_listenerPos.z);
#endif
    gmin = g1min = g2min = g3min = 0.0;
    
    for(i = 0; i < m_nSources; i++) {
        
        x = (*m_sourcePos)[i].x;
        y = (*m_sourcePos)[i].y;
        z = (*m_sourcePos)[i].z;
        
        if (!(*m_sourcePos)[i].isInfinite) {
            x -= m_listenerPos.x;
            y -= m_listenerPos.y;
            z -= m_listenerPos.z;
        }


        if (m_array->is2D()) z = 0; //! temp fix. no fade from 2D plane.
        
        for(j = 0; j < m_array->m_nSpeakers; j++) m_gain[i][j] = 0;
        
        for(j = 0; j < m_array->m_nTriplets; j++) {

            if (m_array->m_triplet[j][0] == -1) continue;  //  triplet unused

            inv = m_invMatrix[j];
            g1 = x*inv[0] + y*inv[1] + z*inv[2];
            g2 = x*inv[3] + y*inv[4] + z*inv[5];
            g3 = x*inv[6] + y*inv[7] + z*inv[8];

            if ( g1 >= 0 && g2 >= 0 && ( g3 >= 0 || m_array->is2D() ) )  // inside triplet, or edge in 2D case.
            {
                jmin = j;
                g1min = g1; g2min = g2; g3min = g3;
                break;  // should only be at most one triplet with all positive gains.
            }
        
            // Update gmin if lowest gain in triplet is higher than gmin.
            // Triplet must have at most 1 -ve gain.
            //! failure possible: if panning outside a 'naked corner' / large z swapping to other triangles.
            //! better geometric solution needed.
            //! 'dead'-speakers provide a posssible solution.

            if ( ( (g1 < 0) + (g2 < 0) + (g3 < 0) <= 1) &&   // at most one negative gain
                 ( jmin == -1 || (g1 > gmin && g2 > gmin && g3 > gmin)  )
            ) {
                jmin = j;
                g1min = g1; g2min = g2; g3min = g3;
                gmin = g1;
                if (g2 < gmin) { gmin = g2; }
                if (g3 < gmin) { gmin = g3; }
            }
        }

        if (jmin == -1) return -1; // failure to find best triplet. No gains set. See //! above
    
        // gains of triplet with highest minimum-gain-in-triplet
        g1 = g1min; g2 = g2min; g3 = g3min;
        

        // in 2D case g3 != 0 when source is out of 2D plane.
        // Normalization causes fade with distance from plane unless g3 set to 0 first.
        //if (m_array->is2D()) g3 = 0;
        

        // Normalization
        // normalization first makes negative gain comparisons more reasonable.
        g = sqrt(g1*g1+g2*g2+g3*g3);
        // g = g1+g2+g3; //! more appropriate for low freq sounds / close speakers.
        if (g > 0) {
          g1 = g1 / g;
          g2 = g2 / g;
          g3 = g3 / g;
        }


        // Remove -ve gain if present, moves image to edge of triplet.
        // after normalization: gains fade with distance from boundary edge.
        // before normalization: gain fixed whether on or off a boundary edge.
        if (g1 < 0) g1 = 0;
        if (g2 < 0) g2 = 0;
        if (g3 < 0) g3 = 0;
        
        // Apply gain limit
        if (g1 > m_maxGain) g1 = m_maxGain;
        if (g1 < -m_maxGain) g1 = -m_maxGain;
        if (g2 > m_maxGain) g2 = m_maxGain;
        if (g2 < -m_maxGain) g2 = -m_maxGain;
        if (g3 > m_maxGain) g3 = m_maxGain;
        if (g3 < -m_maxGain) g3 = -m_maxGain;
        
        l1 = m_array->m_triplet[jmin][0];
        l2 = m_array->m_triplet[jmin][1];
        l3 = m_array->m_triplet[jmin][2];
        m_gain[i][l1] = g1;
        m_gain[i][l2] = g2;
        if (!m_array->is2D()) m_gain[i][l3] = g3;     // l3 undefined in 2D case 
#ifdef VBAP_DEBUG_MESSAGES
        printf("%d  %f %f %f   %d  %d %d %d  %f %f %f \n",i, x,y,z,  jmin, l1,l2,l3, g1,g2,g3);
#endif
    }
    return 0;
}
