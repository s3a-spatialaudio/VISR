//
//  Vbap.h
//  S3A_renderer_dsp
//
//  Created by dylan on 10/11/2014.
//  Copyright (c) 2014 ISVR, Southampton University. All rights reserved.
//

#ifndef __S3A_renderer_dsp__Vbap__
#define __S3A_renderer_dsp__Vbap__



#include "defs.h"
#include "LoudspeakerArray.h"

#define MAX_NUM_SOURCES 512


// #include <iostream>



class Vbap
{
private:
    
    LoudspeakerArray* m_array;
    Afloat m_invMatrix[MAX_NUM_TRIPLETS][9];
    
    Afloat m_gain[MAX_NUM_SOURCES][MAX_NUM_SPEAKERS];
    
    Xyz m_listenerPos;
    
    
public:
    
    int setLoudspeakerArray(LoudspeakerArray* array){
        m_array = array;
    }
    
    int calcInvMatrices();
    
    
};





#endif /* defined(__S3A_renderer_dsp__Vbap__) */
