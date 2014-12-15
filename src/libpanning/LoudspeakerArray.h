//
//  LoudspeakerArray.h
//  S3A_renderer_dsp
//
//  Created by Dylan Menzies on 18/11/2014.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//


#ifndef __S3A_renderer_dsp__LoudspeakerArray__


#define __S3A_renderer_dsp__LoudspeakerArray__

//#include <iostream>
#include <stdio.h>

#include "defs.h"
#include "XYZ.h"

#define MAX_NUM_SPEAKERS 128
#define MAX_NUM_LOUDSPEAKER_TRIPLETS 256



class LoudspeakerArray
{
    
private:


public:
    
    int m_nSpeakers = 0;
    int m_nTriplets = 0;
    
    XYZ m_position[MAX_NUM_SPEAKERS];
    int m_triplet[MAX_NUM_LOUDSPEAKER_TRIPLETS][3];
    
    
    int load(FILE *file);
    
    
    int setPosition(int iSpk, Afloat x, Afloat y, Afloat z) {
        m_position[iSpk].set(x, y, z);
        return 0;
    };
    
    XYZ* getPosition(int iSpk) { return &m_position[iSpk]; };
    
    XYZ (*getPositions())[MAX_NUM_SPEAKERS] { return &m_position; };
    
    int setTriplet(int iTri, int l1, int l2, int l3) {
        m_triplet[iTri][0] = l1;
        m_triplet[iTri][1] = l2;
        m_triplet[iTri][2] = l3;
        return 0;
    }
    
    int* getTriplet(int iTri) { return &m_triplet[iTri][0]; }
    
    int getNumSpeakers() { return m_nSpeakers; };

};

#endif /* defined(__S3A_renderer_dsp__LoudspeakerArray__) */
