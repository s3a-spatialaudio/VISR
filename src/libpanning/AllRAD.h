//
//  AllRAD.h
//
//  Calculate HOA decode matrix by feeding regular HOA decode to external VBAP of virtual louspeakers.
//  Suitable for fast update for listener tracking.
//
//  Created by Dylan Menzies on 12/12/2014.
//  Copyright (c) ISVR, University of Southampton. All rights reserved.
//

#ifndef __S3A_renderer_dsp__AllRAD__
#define __S3A_renderer_dsp__AllRAD__


#define MAX_ORDER 10
#define MAX_NUM_HARMONICS (1+MAX_ORDER)*(1+MAX_ORDER)



#include <iostream>

#include "LoudspeakerArray.h"
#include "VBAP.h"

#include <libefl/basic_matrix.hpp>

namespace visr
{
namespace panning
{

class AllRAD {
private:
    
    // Harmonic ordering is "Ambisonic Channel Numbering" (ACN) i = n^2 + n + m for Y_i = Y_{n m}
    efl::BasicMatrix<Afloat> m_regDecode;

    efl::BasicMatrix<Afloat> m_decode;

    std::size_t m_nHarms = 0;
    std::size_t m_nSpkSources = 0;
    
    LoudspeakerArray* m_regArray;

public:
    
    int setRegArray(LoudspeakerArray* array) { m_regArray = array; return 0; };
    
    int loadRegDecodeGains(FILE* file, int nHarms, int nSpks);
    
    int calcDecodeGains(VBAP* vbap); // Ensure calcInvMatrices is called on vbap first.
    
    efl::BasicMatrix<Afloat> const & getDecodeGains() const {
        return m_decode;
    }
    
};

} // namespace panning
} // namespace visr

#endif /* defined(__S3A_renderer_dsp__AllRAD__) */
