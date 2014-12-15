//
//  AllRAD.h
//  S3A_renderer_dsp
//
//  Calculate HOA decode matrix by feeding regular HOA decode to external VBAP of virtual louspeakers.
//  Suitable for fast update for listener tracking.
//
//  Created by dylan on 12/12/2014.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//

#ifndef __S3A_renderer_dsp__AllRAD__
#define __S3A_renderer_dsp__AllRAD__


#define MAX_ORDER 10
#define MAX_NUM_HARMONICS (1+MAX_ORDER)*(1+MAX_ORDER)



#include <iostream>

#include "LoudspeakerArray.h"
#include "VBAP.h"


class AllRAD {
private:
    
    // Harmonic ordering is "Ambisonic Channel Numbering" (ACN) i = n^2 + n + m for Y_i = Y_{n m}
    Afloat m_regDecode[MAX_NUM_HARMONICS][MAX_NUM_SPEAKERS];
    Afloat m_decode[MAX_NUM_HARMONICS][MAX_NUM_SPEAKERS];
    
    int m_nHarms = 0;
    int m_nSpkSources = 0;
    
    LoudspeakerArray m_regArray;
    
    int m_nSpks = 0;
    
public:
    
    int loadRegArray(FILE* file){
        return m_regArray.load(file);
    }
    
    int loadRegDecodeGains(FILE* file, int nHarms, int nSpks);
    
    int calcDecodeGains(VBAP* vbap); // Ensure calcInvMatrix is called on vbap first.
    
    Afloat (*getDecodeGains())[MAX_NUM_HARMONICS][MAX_NUM_SPEAKERS] {
        return &m_decode;
    }
    
};


#endif /* defined(__S3A_renderer_dsp__AllRAD__) */
