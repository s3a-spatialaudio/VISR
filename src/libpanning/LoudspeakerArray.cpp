//
//  LoudspeakerArray.cpp
//  S3A_renderer_dsp
//
//  Created by Dylan Menzies on 18/11/2014.
//  Copyright (c) 2014 ISVR, Southampton University. All rights reserved.
//

//#include <stdlib.h>
#include "LoudspeakerArray.h"


int LoudspeakerArray::readFile(FILE *file)
{
    //system("pwd");
    int n,i,err;
    char c;
    Afloat x,y,z;
    int l1,l2,l3;
    
    if (file == 0) return -1;

    do {
        
        fscanf(file, "%c", &c);
        if (c == 'l') {
            n = fscanf(file, "%d %f %f %f\n", &i, &x, &y, &z);
            if (i <= MAX_NUM_SPEAKERS) setPosition(i-1,x,y,z);
        }
        else if (c == 't') {
            n = fscanf(file, "%d %d %d %d\n", &i, &l1, &l2, &l3);
            if (i <= MAX_NUM_LOUDSPEAKER_TRIPLETS) setTriplet(i-1, l1, l2, l3);
        }
        
        err = feof(file);

    } while (!err && i <= MAX_NUM_SPEAKERS);
    
    //setNumSpeakers(i);
    
    return 0;
}