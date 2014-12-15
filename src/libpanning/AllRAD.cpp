//
//  AllRAD.cpp
//  S3A_renderer_dsp
//
//  Created by dylan on 12/12/2014.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//

#include "AllRAD.h"


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
    m_nSpks = nSpks;
    
    return 0;
};


//int LoudspeakerArray::load(FILE *file)
//{
//    //system("pwd");
//    int n,i,j,err;
//    char c;
//    Afloat x,y,z;
//    int l1,l2,l3;
//    int nSpk, nTri;
//    
//    i = j = nSpk = nTri = 0;
//    
//    if (file == 0) return -1;
//    
//    do {
//        fscanf(file, "%c",&c);
//        if (c == 'l') {
//            n = fscanf(file, "%d %f %f %f\n", &i, &x, &y, &z);
//            if (i <= MAX_NUM_SPEAKERS) {
//                setPosition(i-1,x,y,z);
//                if (i > nSpk) nSpk = i;
//            }
//        }
//        else if (c == 't') {
//            n = fscanf(file, "%d %d %d %d\n", &j, &l1, &l2, &l3);
//            if (j <= MAX_NUM_LOUDSPEAKER_TRIPLETS) {
//                setTriplet(j-1, l1-1, l2-1, l3-1);
//                if (j > nTri) nTri = j;
//            }
//        }
//        
//        err = i > MAX_NUM_SPEAKERS || j > MAX_NUM_LOUDSPEAKER_TRIPLETS;
//        
//    } while ( !feof(file) && !err );
//    
//    m_nSpeakers = nSpk;
//    m_nTriplets = nTri;
//    
//    if (err) return -1;
//    return 0;
//}




int AllRAD::calcDecodeGains(VBAP* vbap){
    
    Afloat (*vbapGain)[MAX_NUM_SOURCES][MAX_NUM_SPEAKERS];
    int i,j,k,sum;
    
    // In vbap first do externally: setListenerPosition, then calcInvMatrix.
    
    //* This casting could cause failure if MAX_NUM_SOURCES < MAX_NUM_SPEAKERS
    vbap->setSourcePositions(
    (XYZ (*)[MAX_NUM_SOURCES]) m_regArray.getPositions()  );
    
    vbap->calcGains();
    
    vbapGain = vbap->getGains();
    
    // Find decode gains by matrix multiplication
    
    for( i = 0; i < m_nHarms; i++) {
        for( j = 0; j < m_nSpks; j++) {
            sum = 0;
            for( k = 0; k < m_nSpkSources; k++) {
                sum += m_regDecode[i][k] * (*vbapGain)[k][j];
            }
            m_decode[i][j] = sum;
        }
    }
    return 0;
};




