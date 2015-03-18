//
//  FIR.cpp
//
//  Multichannel FIR with one input, multiple outputs.
//
//  Created by Dylan Menzies on 09/03/2015.
//  Copyright (c) ISVR, University of Southampton. All rights reserved.
//


#include <random>
#include "FIR.hpp"



FIR::FIR(){
    int i;
    for(i = 0; i < nBufferSamples; i++) m_inBuffer[i] = 0;
}


int FIR::loadFIRs(FILE* file) {
   
    int i,j;
    
    for( i = 0; i < m_nFIRs; i++ ) {
        for( j = 0; j < m_nFIRsamples; j++ ) {
            if (feof(file) == -1) {
                return -1;
            }
            fscanf(file, "%f", &(m_B[i][j]));
        }
    }
    
    return 0;
}


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
                if (rand01(gen)) { m_B[i][j] = +1; m_iBplus[i][m_nBplus[i]] = j; m_nBplus[i]++; }   //! Better to ensure fixed number of non-zero samples per FIR
                            else { m_B[i][j] = -1; m_iBminus[i][m_nBminus[i]] = j; m_nBminus[i]++; }
            }
            else m_B[i][j] = 0;
        }
        m_gain[i] = 1/sqrt(m_nBplus[i] + m_nBminus[i]); // normalization
    }
    
    return 0;
}



int FIR::process(Afloat (*in)[nBlockSamples], Afloat (*out)[maxnFIRs][nBlockSamples]) {
    
    int i,j,k,iBuf;
    
    // Set input
    for( i = 0; i < nBlockSamples; i++ ) {
        m_inBuffer[m_iBuf + i] = (*in)[i];
    }

    // Calc output
    for( i = 0; i < nBlockSamples; i++ ) {
        
        for( j = 0; j < m_nFIRs; j++ ) {
            (*out)[j][i] = 0;
        }

        iBuf = m_iBuf + i;
        
        for( j = 0; j < m_nFIRsamples; j++ ) {
            
            for( k = 0; k < m_nFIRs; k++ ) {
                (*out)[k][i] += m_B[k][j] * m_inBuffer[iBuf];   // Convolve
            }
            
            iBuf -= m_nUpsample;
            if (iBuf < 0) iBuf += nBufferSamples;
        }
        
    }

    m_iBuf += nBlockSamples;
    if (m_iBuf >= nBufferSamples) m_iBuf = 0;
    
    return 0;
}
