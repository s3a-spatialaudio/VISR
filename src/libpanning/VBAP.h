//
//  VBAP.h
//
//  Created by Dylan Menzies on 10/11/2014.
//  Copyright (c) ISVR, University of Southampton. All rights reserved.
//

#ifndef __S3A_renderer_dsp__VBAP__
#define __S3A_renderer_dsp__VBAP__

// uncomment to enable debug output
// #define VBAP_DEBUG_MESSAGES

#include "defs.h"
#include "LoudspeakerArray.h"

#define MAX_NUM_SOURCES 512


// #include <iostream>

namespace visr
{
namespace panning
{

class VBAP
{
private:
    
    LoudspeakerArray* m_array;
    Afloat m_invMatrix[MAX_NUM_LOUDSPEAKER_TRIPLETS][9];
    Afloat m_gain[MAX_NUM_SOURCES][MAX_NUM_SPEAKERS];
    XYZ m_listenerPos;
    XYZ (*m_sourcePos)[MAX_NUM_SOURCES];
    int m_nSources;
    Afloat m_maxGain = 10.0f;
    
    
    
public:
    
    int setLoudspeakerArray(LoudspeakerArray* array){
        m_array = array;
        return 0;
    }
    
    int setListenerPosition(Afloat x, Afloat y, Afloat z){

#ifdef VBAP_DEBUG_MESSAGES
      printf("setListenerPosition %f %f %f\n",x,y,z);
#endif

        m_listenerPos.set(x, y, z);

        return 0;
    }
    
    int calcInvMatrices();
    
    int setSourcePositions(XYZ (*sp)[MAX_NUM_SOURCES]) {
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
    
    int setMaxGain(Afloat mg) { m_maxGain = mg; return 0; }
    
    std::size_t getNumSpeakers() const { return m_array->getNumSpeakers(); }
    
    int calcGains();
    
    Afloat (*getGains())[MAX_NUM_SOURCES][MAX_NUM_SPEAKERS] {
        return &m_gain;
    }
    
};

} // namespace panning
} // namespace visr

#endif /* defined(__S3A_renderer_dsp__VBAP__) */
