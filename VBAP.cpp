//
//  VBAP.cpp
//  l3A_renderer_dsp
//
//  Created by Dylan Menzies on 10/11/2014.
//  Copyright (c) 2014 ISVR, Southampton University. All rights reserved.
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
        l3 = m_array->m_position[m_array->m_triplet[i][2]];

        
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
    int i,j,l1,l2,l3;
    Afloat* inv;
    Afloat g1,g2,g3,g,x,y,z;
    
    for(i = 0; i < m_nSources; i++) {
        
        x = m_sourcePos[i].x - m_listenerPos.x;
        y = m_sourcePos[i].y - m_listenerPos.y;
        z = m_sourcePos[i].z - m_listenerPos.z;
        
        for(j = 0; j < m_array->m_nSpeakers; j++) m_gain[i][j] = 0;
        
        for(j = 0; j < m_array->m_nTriplets; j++) {
            inv = m_invMatrix[j];
            g1 = x*inv[0] + y*inv[1] + z*inv[2];
            g2 = x*inv[3] + y*inv[4] + z*inv[5];
            g3 = x*inv[6] + y*inv[7] + z*inv[8];
            
            if (g1 >= 0 && g2 >= 0 && g3 >= 0) { //* Slow triplet search
                // g = g1+g2+g3; //* probably more appropriate.
                g = sqrt(g1*g1+g2*g2+g3*g3);
                g1 = g1 / g;
                g2 = g2 / g;
                g3 = g3 / g;
                l1 = m_array->m_triplet[j][0];
                l2 = m_array->m_triplet[j][1];
                l3 = m_array->m_triplet[j][2];
                m_gain[i][l1] += g1;
                m_gain[i][l2] += g2;
                m_gain[i][l3] += g3;
            }
        }

        
    }
    return 0;
}