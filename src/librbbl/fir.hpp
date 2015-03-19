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

#include <libefl/basic_matrix.hpp>

#include <cstdio>

// uncomment to include the tristate FIR stuff again which is not currently used in the filtering.
#define USE_TRISTATE_FIR

namespace visr
{
// forward declarations
namespace efl
{
template<typename ElementType > class BasicMatrix;
}

namespace rbbl
{

class FIR
{
public:
    /**
     * Local typedef for the sample type used in the filter.
     */
    using Afloat = float;

    /**
     * @note This shouldn't be a constant.
     */
    static const int nBlockSamples = 64; //128 //2 test
    
    static const int maxnFIRs = 64;
    /**
     * @note This constant is just used to calculate the buffer sizes, but never set or checked.
     * IMHO, it is always identical to the total length of the FIR filter (which is not required to be an integer multiple of the block length)
     */
    static const int maxnFIRblocks = 32; //32
    static const int maxnFIRsamples = nBlockSamples*maxnFIRblocks;
    static const int nBufferSamples = nBlockSamples*(maxnFIRblocks+1);
    
    FIR();
    
    int setNumFIRs(int n) {
        m_nFIRs = n;
        m_B.resize( n, m_nFIRsamples );
        return 0;
    };
    int setNumFIRsamples(int n) {
        m_nFIRsamples = n;
        m_B.resize( m_nFIRs, m_nFIRsamples );
        m_nFIRsamples = n; return 0;
    };
    int setUpsampleRatio(int n) {
        // <af> Is this correct? m_B is never accesses past m_nFIRsamples ?
        if (n * m_nFIRsamples > maxnFIRsamples) return -1;
        m_nUpsample = n; return 0; };
    
    int loadFIRs(FILE* file);

    /**
     * Load the filter coefficients from a matrix.
     * @param filterCoeffs Matrix of filter coefficients with \p m_nFirs rows and \p m_nFIRsamples columns.
     * @throw std::invalid_argument if the matrix dimensions do not match the settings of the filter.
     */
    void loadFIRs( efl::BasicMatrix<Afloat> const & filterCoeffs );

#ifdef USE_TRISTATE_FIR
    int createWhiteTristateFIRs(Afloat density);
#endif

    int process( Afloat const * in, Afloat * const * out);

    
private:
    
    int m_nFIRs = 0;
    int m_nFIRsamples = 0;
    
    // general FIR
    //! for now fixed memory:
    // Afloat m_B[maxnFIRs][maxnFIRsamples];     // FIR coefficients for each FIR
    efl::BasicMatrix<Afloat> m_B;

    int m_nUpsample = 1;
    
#ifdef USE_TRISTATE_FIR
    //  tristate FIR
    int m_iBplus[maxnFIRs][maxnFIRsamples];   // index list
    int m_iBminus[maxnFIRs][maxnFIRsamples];
    int m_nBplus[maxnFIRs];
    int m_nBminus[maxnFIRs];
#endif
    
    Afloat m_gain[maxnFIRs];                        // for normalization etc.

    Afloat m_inBuffer[nBufferSamples];          // circular input buffer
    int m_iBuf = 0;                             // buffer index to current block.
};

} // namespace rbbl
} // namespace visr

#endif /* defined(__VISR__FIR__) */
