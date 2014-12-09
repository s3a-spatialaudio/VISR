//
//  VBAP.h
//  S3A_renderer_dsp
//
//  Created by Dylan Menzies on 10/11/2014.
//  Copyright (c) 2014 ISVR, Southampton University. All rights reserved.
//

#ifndef __S3A_renderer_dsp__VBAP__
#define __S3A_renderer_dsp__VBAP__



#include "defs.h"
#include "LoudspeakerArray.h"

#define MAX_NUM_SOURCES 512


// #include <iostream>



class VBAP
{
private:
    
    LoudspeakerArray* m_array;
    Afloat m_invMatrix[MAX_NUM_LOUDSPEAKER_TRIPLETS][9];
    Afloat m_gain[MAX_NUM_SOURCES][MAX_NUM_SPEAKERS];
    XYZ m_listenerPos;
    XYZ* m_sourcePos;
    int m_nSources;
    
    
public:
    
    int setLoudspeakerArray(LoudspeakerArray* array){
        m_array = array;
        return 0;
    }
    
    int setListenerPos(Afloat x, Afloat y, Afloat z){
        m_listenerPos.set(x, y, z);
        return 0;
    }
    
    int calcInvMatrices();
    
    int setSourcePos(XYZ* sp) {
        m_sourcePos = sp;
        return 0;
    }
    
    int setNumSources(int n) {
        if (n > MAX_NUM_SOURCES) {
            m_sourcePos = 0;
            return 0;
        }
        m_nSources = n;
        return n;
    }
    
    int calcGains();
    
    Afloat* getGains() {
        return &(m_gain[0][0]);
    }
    
};





#endif /* defined(__S3A_renderer_dsp__VBAP__) */
