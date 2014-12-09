//
//  LoudspeakerArray.cpp
//  S3A_renderer_dsp
//
//  Created by dylan on 18/11/2014.
//  Copyright (c) 2014 ISVR, Southampton University. All rights reserved.
//

//#include <stdlib.h>
#include "LoudspeakerArray.h"


int LoudspeakerArray::readFile(FILE *file)
{
    //system("pwd");
    int n,i,err,iSpk;
    char c;
    Xyz pos;
    // Triangle tri;
    
    if (file==0) return -1;

    do {
        
        fscanf(file, "%c", &c);
        if (c == 's') {
            n = fscanf(file, "%d %f %f %f\n", &i, &pos[0],&pos[1],&pos[2]);
            if (i <= MAX_NUM_SPEAKERS) setPosition(i-1, &pos);
        }
        else if (c == 't') {
            n = fscanf(file, "%d %d %d %d\n", &i, &tri[0],&tri[1],&tri[2]);
            
        }
        
        err = feof(file);

    } while (!err && i <= MAX_NUM_SPEAKERS);
    
    //setNumSpeakers(i);
    
    return 0;
}