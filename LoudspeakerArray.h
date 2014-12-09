//
//  LoudspeakerArray.h
//  S3A_renderer_dsp
//
//  Created by dylan on 18/11/2014.
//  Copyright (c) 2014 ISVR, Southampton University. All rights reserved.
//


#ifndef __S3A_renderer_dsp__LoudspeakerArray__


#define __S3A_renderer_dsp__LoudspeakerArray__

//#include <iostream>
#include <stdio.h>

#include "defs.h"

#define MAX_NUM_SPEAKERS 128
#define MAX_NUM_SPEAKER_TRIPLETS 256


class LoudspeakerArray
{
    
private:

    
public:
    
    int m_nSpeakers = 0;
    int m_nTriplets = 0;
    
    Xyz m_position[MAX_NUM_SPEAKERS];
    bool m_isDirectionMode = false;
    int m_triplet[MAX_NUM_SPEAKER_TRIPLETS][3];
    
    
    
    int readFile(FILE *file);
    
    
//    int setPosition(int iSpk, Xyz *pos){ m_position[iSpk] = *pos; };
//    Xyz* getPosition(int iSpk){ return &m_position[iSpk]; };
//    int setTriplet(int iTri, Xyz *pos);
//    int getTriplet(int iTri, Xyz *pos);

};

#endif /* defined(__S3A_renderer_dsp__LoudspeakerArray__) */
