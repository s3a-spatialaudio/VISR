//
//  main.cpp
//  S3A_renderer_dsp
//
//  Created by Dylan Menzies on 10/11/2014.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//


#include <libpanning/VBAP.h>
#include <libpanning/AllRAD.h>

#include <iostream>
#include <cstdio>

int main(int argc, const char * argv[])
{
    LoudspeakerArray array;
    VBAP vbap;
    AllRAD allrad;
    Afloat (*vbapGains)[MAX_NUM_SOURCES][MAX_NUM_SPEAKERS];
    Afloat (*decodeGains)[MAX_NUM_HARMONICS][MAX_NUM_SPEAKERS];
    XYZ sourcePos[MAX_NUM_SOURCES];

    FILE* file;
    int j,k;
    
    
    
    // Useage / test VBAP with 8 sources around an octahedron array
    
    file = fopen("octahedron.txt","r");
    if (array.load(file) == -1) return -1;
    fclose( file );
    file = 0;

    vbap.setLoudspeakerArray(&array);
    vbap.setListenerPosition(0.0, 0.0, 0.0);
    vbap.calcInvMatrices();
    
    vbap.setNumSources(10);
    sourcePos[0].set(1.0, 1.0, 1.0, false);
    sourcePos[1].set(1.0, 0.0, 0.0, false);
    sourcePos[2].set(0.0, 1.0, 0.0, false);
    sourcePos[3].set(0.0, 0.0, 1.0, false);
    sourcePos[4].set(-1.0, 0.0, 0.0, false);
    sourcePos[5].set(0.0, -1.0, 0.0, false);
    sourcePos[6].set(0.0, 0.0, -1.0, false);
    sourcePos[7].set(-1.0, -1.0, -1.0, false);
    vbap.setSourcePositions(&sourcePos);

    vbap.calcGains();
    
    vbapGains = vbap.getGains();   // Check in watch window
    
    
    // 5.1 test, 2D VBAP
    
    file = fopen("5.1.txt","r");
    if (array.load(file) == -1) return -1;
    fclose( file );
    file = 0;
    
    vbap.setLoudspeakerArray(&array);
    vbap.setListenerPosition(0.0, 0.0, 0.0);
    vbap.calcInvMatrices();
    
    vbap.setNumSources(4);
    sourcePos[0].set(1.0, 0.0, 0.0, false);
    sourcePos[1].set(0.0, 1.0, 0.0, false);
    sourcePos[2].set(-1.0, 0.0, 0.0, false);
    sourcePos[3].set(0.0, -1.0, 0.0, false);
    vbap.setSourcePositions(&sourcePos);
    
    vbap.calcGains();
    
    vbapGains = vbap.getGains();   // Check in watch window
    
    
    // Useage / test AllRAD ambisonic decode
    
    
    // Initialization:
    
    file = fopen("octahedron.txt","r");
    if (array.load(file) == -1) return -1;
    fclose( file );
    file = 0;
    vbap.setLoudspeakerArray(&array);
    
    
    file = fopen("t-design_t=8_40point.txt","r");
    if (allrad.loadRegArray(file) == -1) return -1;
    fclose( file );
    file = 0;
    
    
    file = fopen("decode_N8_P40_t-design_8_40.txt","r");
    if (allrad.loadRegDecodeGains(file, 3, 40) == -1) return -1;
    fclose( file );
    file = 0;
    
    
    // Initially and every time listener moves:
    
    vbap.setListenerPosition(0.0, 0.0, 0.0);
    vbap.calcInvMatrices();
    
    // Load vbap with other sources and find loudspeaker gains
    // ....
    // ....
    
    
    
    // Load vbap with reg-array-speaker-sources
    // Calc vbap gains then calc AllRAD decode gains
    
    allrad.calcDecodeGains(&vbap);
    decodeGains = allrad.getDecodeGains();
    
    file = fopen("/Users/rdmg1u13/Dropbox/s3a/research/S3A_renderer/visr/src/libpanning/test/matlab/decodeB2VBAP.txt","w");
    if( file ) {
        for(k=0; k<9; k++) {
            for(j=0; j<vbap.getNumSpeakers(); j++) {
                fprintf(file, "%f ", (*decodeGains)[k][j]);
            }
            fprintf(file,"\n");
        }
        fclose( file );
        file = 0;
    }
    
    // std::cout << "Hello, World!\n";
    return 0;
}

