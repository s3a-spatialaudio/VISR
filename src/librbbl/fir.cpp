//
//  FIR.cpp
//
//  Multichannel FIR with one input, multiple outputs.
//
//  Created by Dylan Menzies on 09/03/2015.
//  Copyright (c) ISVR, University of Southampton. All rights reserved.
//


#include "fir.hpp"

#include <libefl/basic_matrix.hpp>

#include <random>

namespace visr
{
namespace rbbl
{

  FIR::FIR()
  : m_B( 1 ) // no special alignment for the moment.
  , m_nUpsample( 1 ) // start with a sensible value (0 is not!)
{
    int i;
    for(i = 0; i < nBufferSamples; i++) m_inBuffer[i] = 0;
}

void FIR::loadFIRs( efl::BasicMatrix<Afloat> const & filterCoeffs )
{
  if( (filterCoeffs.numberOfRows() != static_cast<std::size_t>(m_nFIRs))
      or (filterCoeffs.numberOfColumns() != static_cast<std::size_t>(m_nFIRsamples)) )
  {
    throw std::invalid_argument( "FIR::loadFIRs: The dimension of the filterCoeffs parameter does not match the dimension of the filter.");
  }
  m_B.copy( filterCoeffs );
}

#ifdef USE_TRISTATE_FIR
int FIR::createWhiteTristateFIRs(Afloat density){
    
    std::default_random_engine gen;
    std::uniform_real_distribution<double> rand(0.0,1.0);
    std::uniform_int_distribution<int> rand01(0,1);
    
    int i,j;

    
    for( i = 0; i < m_nFIRs; i++ ) {
        m_nBplus[i] = 0;
        m_nBminus[i] = 0;
        for( j = 0; j < m_nFIRsamples; j++ ) {
            if (rand(gen) <= density) {
                if (rand01(gen)) { m_B(i,j) = +1; m_iBplus[i][m_nBplus[i]] = j; m_nBplus[i]++; }   //! Better to ensure fixed number of non-zero samples per FIR
                            else { m_B(i,j) = -1; m_iBminus[i][m_nBminus[i]] = j; m_nBminus[i]++; }
            }
            else m_B(i,j) = 0;
        }
        m_gain[i] = 1.0f/std::sqrt(static_cast<float>(m_nBplus[i] + m_nBminus[i])); // normalization
    }
    
    return 0;
}
#endif


int FIR::process( Afloat const * in, Afloat * const * out) {
    
    int i,j,k,iBuf;
    
    // Set input
    for( i = 0; i < nBlockSamples; i++ ) {
        m_inBuffer[m_iBuf + i] = in[i];
    }

    // Calc output
    for( i = 0; i < nBlockSamples; i++ ) {
        
        for( j = 0; j < m_nFIRs; j++ ) {
            out[j][i] = 0;
        }

        iBuf = m_iBuf + i;
        
        for( j = 0; j < m_nFIRsamples; j++ ) {
            
            for( k = 0; k < m_nFIRs; k++ ) {
                out[k][i] += m_B(k,j) * m_inBuffer[iBuf];   // Convolve
            }
            
            iBuf -= m_nUpsample;
            if (iBuf < 0) iBuf += nBufferSamples;
        }
        
    }

    m_iBuf += nBlockSamples;
    if (m_iBuf >= nBufferSamples) m_iBuf = 0;
    
    return 0;
}

} // namespace rbbl
} // namespace visr
