//
//  VBAP.cpp
//
//  Created by Dylan Menzies on 10/11/2014.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//

#include "VBAP.h"



int VBAP::calcInvMatrices(){
    XYZ l1, l2, l3;
    Afloat det;
    Afloat* inv;
    int i;
    
    
        
    for (i = 0 ; i < m_array->m_nTriplets; i++) {
            
            l1 = m_array->m_position[m_array->m_triplet[i][0]];
            l2 = m_array->m_position[m_array->m_triplet[i][1]];
            if (m_array->is2D()) { l3.x = 0; l3.y = 0; l3.z = 1; }  // adapt 3D calc for 2D array
            else l3 = m_array->m_position[m_array->m_triplet[i][2]];
        
            det = 1.0 / (  l1.x * ((l2.y * l3.z) - (l2.z * l3.y))
                         - l1.y * ((l2.x * l3.z) - (l2.z * l3.x))
                         + l1.z * ((l2.x * l3.y) - (l2.y * l3.x)));
            
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
    Afloat g1,g2,g3,g,gmin,x,y,z;
    
    //! Slow triplet search
    // Find triplet with highest minimum-gain-in-triplet
    
    for(i = 0; i < m_nSources; i++) {
        
        x = (*m_sourcePos)[i].x;
        y = (*m_sourcePos)[i].y;
        z = (*m_sourcePos)[i].z;
        
        if (!(*m_sourcePos)[i].isInfinite) {
            x -= m_listenerPos.x;
            y -= m_listenerPos.y;
            z -= m_listenerPos.z;
        }
        
        for(j = 0; j < m_array->m_nSpeakers; j++) m_gain[i][j] = 0;
        
        for(j = 0; j < m_array->m_nTriplets; j++) {
            inv = m_invMatrix[j];
            g1 = x*inv[0] + y*inv[1] + z*inv[2];
            g2 = x*inv[3] + y*inv[4] + z*inv[5];
            g3 = x*inv[6] + y*inv[7] + z*inv[8];
            
            
            if (g1 >= 0 && g2 >= 0 && g3 >= 0)
            {
                jmin = j;
                break;  // Should only be one triplet with all positive gains.
            }
        
            // Update gmin if lowest gain in triplet is higher than gmin.
            if ((g1 > gmin && g2 > gmin && g3 > gmin) || j == 0) {
                jmin = j;
                gmin = g1;
                if (g2 < gmin) { gmin = g2; }
                if (g3 < gmin) { gmin = g3; }
            }
            
        }
        
        // Remove -ve gain, moves image inside triplet.
        if (g1 < 0) g1 = 0;
        if (g2 < 0) g2 = 0;
        if (g3 < 0) g3 = 0;
        
        g = sqrt(g1*g1+g2*g2+g3*g3);
        // g = g1+g2+g3; //! probably more appropriate for low freq sounds / close speakers.
        g1 = g1 / g;
        g2 = g2 / g;
        g3 = g3 / g;
        l1 = m_array->m_triplet[jmin][0];
        l2 = m_array->m_triplet[jmin][1];
        l3 = m_array->m_triplet[jmin][2];
        m_gain[i][l1] += g1;
        m_gain[i][l2] += g2;
        m_gain[i][l3] += g3;
        
    }
    return 0;
}