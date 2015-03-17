//
//  fir.hpp
//
//  Created by Dylan Menzies on 09/03/2015.
//  Copyright (c) ISVR, University of Southampton. All rights reserved.
//
//  Multichannel FIR with one input, multiple outputs.
//  includes:
//  - impulse response load
//  - upsample - makes impulse longer with same number of multiplies, spectrum is shrunk and repeated.
//  - create/run sparse white noise tristate FIR - efficient addition-only filters for fast spatial spreading with spectral variation. Suitable for per-object spreading with spectral variation.



#ifndef __VISR__FIR__
#define __VISR__FIR__

#include <cstdio>

namespace visr
{
namespace rbbl
{

typedef float Afloat;


class FIR
{
public:
    
    static const int nBlockSamples = 128; //128 //2 test
    
    static const int maxnFIRs = 64;
    static const int maxnFIRblocks = 32; //32
    static const int maxnFIRsamples = nBlockSamples*maxnFIRblocks;
    static const int nBufferSamples = nBlockSamples*(maxnFIRblocks+1);
    
    FIR();
    
    int setNumFIRs(int n) {
        if (n > maxnFIRs) return -1;
        m_nFIRs = n; return 0;
    };
    int setNumFIRsamples(int n) {
        if (n > maxnFIRsamples) return -1;
        m_nFIRsamples = n; return 0;
    };
    int setUpsampleRatio(int n) {
        if (n * m_nFIRsamples > maxnFIRsamples) return -1;
        m_nUpsample = n; return 0; };
    
    int loadFIRs(FILE* file);
    int createWhiteTristateFIRs(Afloat density);
    
    int process(Afloat (*in)[nBlockSamples], Afloat (*out)[maxnFIRs][nBlockSamples]);

    
private:
    
    int m_nFIRs = 0;
    int m_nFIRsamples = 0;
    
    // general FIR
    //! for now fixed memory:
    Afloat m_B[maxnFIRs][maxnFIRsamples];     // FIR coefficients for each FIR
    
    int m_nUpsample = 1;
    
    
    //  tristate FIR
    int m_iBplus[maxnFIRs][maxnFIRsamples];   // index list
    int m_iBminus[maxnFIRs][maxnFIRsamples];
    int m_nBplus = 0;
    int m_nBminus = 0;
    
    Afloat m_gain[maxnFIRs];                        // for normalization etc.
    
    
    
    Afloat m_inBuffer[nBufferSamples];          // circular input buffer
    int m_iBuf = 0;                             // buffer index to current block.

};

} // namespace rbbl
} // namespace visr

#endif /* defined(__VISR__FIR__) */
