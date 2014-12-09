//
//  XYZ.h
//  S3A_renderer_dsp
//
//  Created by dylan on 09/12/2014.
//  Copyright (c) 2014 ISVR, Southampton University. All rights reserved.
//

#ifndef __S3A_renderer_dsp__XYZ__
#define __S3A_renderer_dsp__XYZ__

#include <iostream>
#include <math.h>
#include "defs.h"

class XYZ {
private:

public:
    Afloat x, y, z;
    bool isAtInfinity; // eg for source at infinity.
    
    int set(Afloat X, Afloat Y, Afloat Z) {
        x = X; y = Y; z =Z;
        return 0;
    };
    
    Afloat getLength() {
        return sqrt(x*x + y*y + z*z);
    }
    
};




#endif /* defined(__S3A_renderer_dsp__XYZ__) */
