//
//  AllRAD.cpp
//
//  Created by Dylan Menzies on 12/12/2014.
//  Copyright (c) ISVR, University of Southampton. All rights reserved.
//

// avoid annoying warning about unsafe STL functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

#include "AllRAD.h"

namespace visr
{
namespace panning
{

int AllRAD::loadRegDecodeGains(FILE* file, int order, int nSpks){
    
    int i,j, nHarms;
    
    nHarms = (order+1)*(order+1);
    
    for( i = 0; i < nHarms; i++ ) {
        for( j = 0; j < nSpks; j++ ) {
            if (feof(file) == -1) {
                return -1;
            }
            fscanf(file, "%f", &(m_regDecode[i][j]));
        }
    }
    
    m_nHarms = nHarms;
    m_nSpkSources = nSpks;
    
    return 0;
};





int AllRAD::calcDecodeGains(VBAP* vbap){
    
    Afloat (*vbapGain)[MAX_NUM_SOURCES][MAX_NUM_SPEAKERS];
    int i,j,k,nSpks;
    Afloat sum;
    
    // In vbap first do externally: setListenerPosition, then calcInvMatrix.
    
    //* This casting could cause failure if MAX_NUM_SOURCES < MAX_NUM_SPEAKERS
    vbap->setSourcePositions(
    (XYZ (*)[MAX_NUM_SOURCES]) m_regArray->getPositions()  );
    
    vbap->setNumSources(m_regArray->getNumSpeakers());
    
    //! set isAtInfinity flag for all regArray speakers
    
    vbap->calcGains();
    
    vbapGain = vbap->getGains();
    
    nSpks = vbap->getNumSpeakers();
    
    // Find decode gains by matrix multiplication
    
    for( i = 0; i < m_nHarms; i++) {
        for( j = 0; j < nSpks; j++) {
            sum = 0;
            for( k = 0; k < m_nSpkSources; k++) {
                sum += m_regDecode[i][k] * (*vbapGain)[k][j];
            }
            m_decode[i][j] = sum;
        }
    }
    return 0;
};

} // namespace panning
} // namespace visr
