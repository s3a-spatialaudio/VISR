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
    
    
int CAP::setListenerOrientation(Afloat yaw, Afloat pitch, Afloat roll, bool zero){
    
        // zero = true  is used to set the identity orientation.
    
    
#ifdef CAP_DEBUG_MESSAGES
        printf("setListenerOrientation %f %f %f %d\n", yaw, pitch, roll, zero);
#endif
        Afloat m[3][3]; // temp rotation matrix
        
        
        // phi the psi
        
        Afloat cy = cos(yaw);
        Afloat sy = sin(yaw);
        Afloat cp = cos(pitch);
        Afloat sp = sin(pitch);
        Afloat cr = cos(roll);
        Afloat sr = sin(roll);
        
        
        m[0][0] = cp * cy;
        m[0][1] = cp * sy;
        m[0][2] = -sp;
        
        m[1][0] = sr * sp * cy - cr * sy;
        m[1][1] = sr * sp * sy + cr * cy;
        m[1][2] = cp * sr;
        
        m[2][0] = cr * sp * cy + sr * sy;
        m[2][1] = cr * sp * sy - sr * cy;
        m[2][2] = cp * cr;
        
        
        if (zero) { // Update reorientation matrix by finding the inverse of current matrix
            Afloat det = m[0][0] * ( m[1][1] * m[2][2] - m[2][1] * m[1][2] )
            - m[0][1] * ( m[1][0] * m[2][2] - m[1][2] * m[2][0] ) +
            m[0][2] * ( m[1][0] * m[2][1] - m[1][1] * m[2][0] );
            
            Afloat invdet = 1 / det;
            
            Afloat (*minv)[3] = m_reorientMatrix; // inverse of matrix m
            minv[0][0] = ( m[1][1] * m[2][2] - m[2][1] * m[1][2] ) * invdet;
            minv[0][1] = ( m[0][2] * m[2][1] - m[0][1] * m[2][2] ) * invdet;
            minv[0][2] = ( m[0][1] * m[1][2] - m[0][2] * m[1][1] ) * invdet;
            minv[1][0] = ( m[1][2] * m[2][0] - m[1][0] * m[2][2] ) * invdet;
            minv[1][1] = ( m[0][0] * m[2][2] - m[0][2] * m[2][0] ) * invdet;
            minv[1][2] = ( m[1][0] * m[0][2] - m[0][0] * m[1][2] ) * invdet;
            minv[2][0] = ( m[1][0] * m[2][1] - m[2][0] * m[1][1] ) * invdet;
            minv[2][1] = ( m[2][0] * m[0][1] - m[0][0] * m[2][1] ) * invdet;
            minv[2][2] = ( m[0][0] * m[1][1] - m[1][0] * m[0][1] ) * invdet;
        }
        
        // Apply reorientation
        
        Afloat m2[3][3]; // temp rotation matrix
        
        int temp = 0;
        int i, j, k;
        
        for(i = 0; i < 3; i++)
        {
            for(j = 0; j < 3; j++)
            {
                temp = 0;
                for(k = 0; k < 3; k++)
                {
                    temp += m_reorientMatrix[j][k] * m[k][i];
                }
                m2[j][i] = temp;
            }
        }
        
        m_listenerAuralAxis.set(m2[1][0], m2[1][1], m2[1][2]);
        
        
        //        a11 = math.cos(the) * math.cos(phi)
        //        a12 = math.cos(the) * math.sin(phi)
        //        a13 = -math.sin(the)
        //
        //        a21 = math.sin(psi) * math.sin(the) * math.cos(phi) - math.cos(psi) * math.sin(phi)
        //        a22 = math.sin(psi) * math.sin(the) * math.sin(phi) + math.cos(psi) * math.cos(phi)
        //        a23 = math.cos(the) * math.sin(psi)
        //
        //        a31 = math.cos(psi) * math.sin(the) * math.cos(phi) + math.sin(psi) * math.sin(phi)
        //        a32 = math.cos(psi) * math.sin(the) * math.sin(phi) - math.sin(psi) * math.cos(phi)
        //        a33 = math.cos(the) * math.cos(psi)
        
        
        
        return 0;
    }

} // namespace panning
} // namespace visr
