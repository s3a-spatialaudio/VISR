//
//  main.cpp
//
//  Created by Dylan Menzies on 10/11/2014.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//

#include <libpanning/VBAP.h>
#include <libpanning/AllRAD.h>
#include <boost/filesystem.hpp>

#include <iostream>
#include <cstdio>

int main(int argc, const char * argv[])
{
    

    LoudspeakerArray array, regArray;
    VBAP vbap;
    AllRAD allrad;
    Afloat (*vbapGains)[MAX_NUM_SOURCES][MAX_NUM_SPEAKERS];
    Afloat (*decodeGains)[MAX_NUM_HARMONICS][MAX_NUM_SPEAKERS];
    XYZ sourcePos[MAX_NUM_SOURCES];

    FILE* file;
    int j,k;
    
    boost::filesystem::path const decoderDir( CMAKE_CURRENT_SOURCE_DIR );
    
    
    // Useage / test VBAP with 8 sources around an octahedron array
    
//    boost::filesystem::path bfile = decoderDir / boost::filesystem::path("arrays/octahedron.txt");
    boost::filesystem::path bfile = decoderDir / boost::filesystem::path("arrays/9.1_audiolab.txt");
//    boost::filesystem::path bfile = decoderDir / boost::filesystem::path("arrays/cube_audiolab.txt");
    file = fopen(bfile.string().c_str(),"r");
    if (array.load(file) == -1) {
       return -1; 
    }
    fclose( file );
    file = 0;

    vbap.setLoudspeakerArray(&array);
    vbap.setListenerPosition(0.0, 0.0, 0.0);
    vbap.calcInvMatrices();
    
//  vbap.setNumSources(1);
//  sourcePos[0].set(1.78, 1.73, -0.86, false);

    vbap.setNumSources(8);
//    sourcePos[0].set(1.0, 1.0, 1.0, false);
// sourcePos[0].set(1.0,	0.3,	-0.2, false); // for 9.1_audiolab.txt jumps between triplet 11 and 10 as z reduced
    sourcePos[0].set(0.0,	1.0,	-0.9, false);
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
    
    // file = fopen("arrays/5.1.txt","r");
    bfile = decoderDir / boost::filesystem::path("arrays/stereo_audiolab.txt");
    //bfile = decoderDir / boost::filesystem::path("arrays/5.1_audiolab.txt");
    file = fopen(bfile.string().c_str(),"r");
    if (array.load(file) == -1) return -1;
    fclose( file );
    file = 0;
    
    vbap.setLoudspeakerArray(&array);
    vbap.setListenerPosition(0.0, 0.0, 0.0);
    vbap.calcInvMatrices();
    
    vbap.setNumSources(4);
    sourcePos[0].set(1.0, 0.0, 0.0, false);
//    sourcePos[0].set(1.0, 0.0, 0.0, true);  // plane wave
    sourcePos[1].set(0.0, 1.0, 0.0, false);
    sourcePos[2].set(-1.0, 0.0, 0.0, false);
    sourcePos[3].set(0.0, -1.0, 0.0, false);
//    sourcePos[0].set(1.0, 0.0, -5.0, false);
//    sourcePos[1].set(0.0, 1.0, -5.0, false);
//    sourcePos[2].set(-1.0, 0.0, -5.0, false);
//    sourcePos[3].set(0.0, -10.0, -50.0, false);


    vbap.setSourcePositions(&sourcePos);
    
    vbap.calcGains();
    
    vbapGains = vbap.getGains();   // Check in watch window
    
    
    
    // Useage / test AllRAD ambisonic decode
    
    // Initialization:
    
    // file = fopen("arrays/octahedron.txt","r");
    bfile = decoderDir / boost::filesystem::path("arrays/octahedron.txt");
    file = fopen(bfile.string().c_str(),"r");
    if (array.load(file) == -1) return -1;
    fclose( file );
    vbap.setLoudspeakerArray(&array);
    
    // file = fopen("arrays/t-design_t8_P40.txt","r");
    bfile = decoderDir / boost::filesystem::path("arrays/t-design_t8_P40.txt");
    file = fopen(bfile.string().c_str(),"r");
    if (regArray.load(file) == -1) return -1;
    allrad.setRegArray(&regArray);
    fclose( file );
    
    // file = fopen("arrays/decode_N8_P40_t-design_t8_P40.txt","r");
    bfile = decoderDir / boost::filesystem::path("arrays/decode_N8_P40_t-design_t8_P40.txt");
    file = fopen(bfile.string().c_str(),"r");
    if (allrad.loadRegDecodeGains(file, 8, 40) == -1) return -1;
    fclose( file );
    
    
    // Initially and every time listener moves:
    
    vbap.setListenerPosition(0.0, 0.0, 0.0);
    vbap.calcInvMatrices();
    
    // Load vbap with other sources and find loudspeaker gains
    // ....
    // ....
    
    
    
    // Load vbap with reg-array-speaker-sources
    // Calc vbap gains then calc AllRAD b-format decode gains
    
    allrad.calcDecodeGains(&vbap);
    decodeGains = allrad.getDecodeGains();
    
    
    // Write b-format2vbap gains for matlab testing:
    
    // file = fopen("testoutput/decodeB2VBAP.txt","w");
    bfile = decoderDir / boost::filesystem::path("testoutput/decodeB2VBAP.txt");
    file = fopen(bfile.string().c_str(),"w");
    if( file ) {
      for(k=0; k<9; k++) { // 9 harms - 2nd order only
        for(j=0; j<vbap.getNumSpeakers(); j++) {
            fprintf(file, "%f ", (*decodeGains)[k][j]);
        }
        fprintf(file,"\n");
      }
      fclose( file );
    }
    return 0;
}

