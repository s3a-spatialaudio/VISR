//
//  Vbap.cpp
//  S3A_renderer_dsp
//
//  Created by dylan on 10/11/2014.
//  Copyright (c) 2014 ISVR, Southampton University. All rights reserved.
//

#include "Vbap.h"



int Vbap::calcInvMatrices(){
    Afloat *s1, *s2, *s3;
    Afloat s1x,s1y,s1z,s2x,s2y,s2z,s3x,s3y,s3z;
    Afloat det;
    int i;
    
    for (i = 0 ; i < nTriplets; i++) {
        s1 = m_array->m_position[m_array->m_triplet[i][0]];
        s2 = m_array->m_position[m_array->m_triplet[i][1]];
        s3 = m_array->m_position[m_array->m_triplet[i][2]];
        s1x = s1[0]; s1y = s1[1]; s1z = s1[2];
        s2x = s2[0]; s2y = s2[1]; s2z = s2[2];
        s3x = s3[0]; s3y = s3[1]; s3z = s3[2];
        
        det = 1.0 / (  s1x * ((s2y * s3z) - (s2z * s3y))
                     - s1y * ((s2x * s3z) - (s2z * s3x))
                     + s1z * ((s2x * s3y) - (s2y * s3x)));
        
        invmx[0] = ((s2y * s3z) - (s2z * s3y)) * det;
        invmx[3] = ((s1y * s3z) - (s1z * s3y)) * -det;
        invmx[6] = ((s1y * s2z) - (s1z * s2y)) * det;
        invmx[1] = ((s2x * s3z) - (s2z * s3x)) * -det;
        invmx[4] = ((s1x * s3z) - (s1z * s3x)) * det;
        invmx[7] = ((s1x * s2z) - (s1z * s2x)) * -det;
        invmx[2] = ((s2x * s3y) - (s2y * s3x)) * det;
        invmx[5] = ((s1x * s3y) - (s1y * s3x)) * -det;
        invmx[8] = ((s1x * s2y) - (s1y * s2x)) * det;
    }


}