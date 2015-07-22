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

#include <libefl/basic_matrix.hpp>

namespace visr
{
namespace panning
{

class VBAP
{
private:
    
    LoudspeakerArray const * m_array;
    efl::BasicMatrix<Afloat> m_invMatrix;
    efl::BasicMatrix<Afloat> m_gain;
    XYZ m_listenerPos;
    XYZ const * m_sourcePos;
    std::size_t m_nSources;
    Afloat m_maxGain = 10.0f;
    
    
    
public:
  /**
   * Default constructor.
   * Sets object to save values (no array, zero sources).
   */
  VBAP();

    int setLoudspeakerArray(LoudspeakerArray const * array){
        m_array = array;
        m_invMatrix.resize( array->getNumTriplets(), 9 );
        m_gain.resize( m_nSources, array->getNumSpeakers( ) );
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
    
    int setSourcePositions(XYZ const *sp ) {
        m_sourcePos = sp;
        return 0;
    }
    
    std::size_t setNumSources( std::size_t n) {
        m_nSources = n;
	// Take care of the fact that the loudspeaker
	// array might not been set yest.
	std::size_t const numSpeakers = m_array ? m_array->getNumSpeakers() : 0;
        m_gain.resize( m_nSources, numSpeakers );
        return n;
    }
    
    int setMaxGain(Afloat mg) { m_maxGain = mg; return 0; }
    
    std::size_t getNumSpeakers() const { return m_array->getNumSpeakers(); }
    
    int calcGains();
    
    efl::BasicMatrix<Afloat> const & getGains() const {
       return m_gain;
    }
    
};

} // namespace panning
} // namespace visr

#endif /* defined(__S3A_renderer_dsp__VBAP__) */
