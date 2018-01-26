//
//  CAP.h
//  Created by Dylan Menzies on 24/1/2018.
//  Copyright (c) ISVR, University of Southampton. All rights reserved.
//
//  Minimum energy Multichannel Compensated Amplitude Panning
//  Panning compensated for head orientation, valid in ITD frequency range ~(0,1000)Hz
//

#include "CAP.h"

#ifdef CAP_DEBUG_MESSAGES
#include <cstdio>
#endif

namespace visr
{
namespace panning
{
  
CAP::CAP()
 : m_array( 0 )
 , m_nSources( 0 )
{
}

int CAP::calcGains(){

    //! Slow triplet search
    // Find triplet with highest minimum-gain-in-triplet (may be negative)

#ifdef CAP_DEBUG_MESSAGES
    printf("setListenerPosition %f %f %f\n",m_listenerPos.x,m_listenerPos.y,m_listenerPos.z);
#endif


    m_gain.zeroFill();
    
    // Abbreviated variables:
    // r    speaker normal vectors
    // rL   inter-aural axis normal vector
    // rI   image normal vector
    
    std::size_t const n = m_array->getNumSpeakers();
    
    
    // Setup normal vectors from listener to speakers
    std::vector<XYZ> r;
    r.resize( n );
    
    for (std::size_t i = 0 ; i < n; i++) {
        r[i] = m_array->getPosition(i);
        r[i].x -= m_listenerPos.x;
        r[i].y -= m_listenerPos.y;
        r[i].z -= m_listenerPos.z;
        r[i].normalise();
    }
    
    XYZ & rL = m_listenerAuralAxis; // assume normalised.
    
    std::vector<Afloat> a;
    a.resize(n);
    Afloat b, c, l1, l2;  // main algorithm variables.
    Afloat t; // temp
    XYZ rt; // temp
    
    for(std::size_t j = 0; j < m_nSources; j++) {
    
        XYZ rI = m_sourcePos[j];
        
        if (!m_sourcePos[j].isInfinite) {
            rI.minus(m_listenerPos);
        }

        rI.normalise();
        
        b = c = 0;
        
        for (std::size_t i = 0 ; i < n; i++) {
            rt = r[i];
            rt.minus(rI);
            a[i] = rL.dot(rt);
            
            b += a[i];
            c += a[i] * a[i];
        }
        
        t = 2/(c*n - b*b);
        l1 = -b * t;
        l2 = c * t;
    
        for (std::size_t i = 0 ; i < n; i++) {
            m_gain(j, i) = ( l1*a[i] + l2 )/2;
            //printf( m_gain(j, i) );
        }

#ifdef CAP_DEBUG_MESSAGES
        printf("\n");
#endif
    }
    return 0;
}

} // namespace panning
} // namespace visr
