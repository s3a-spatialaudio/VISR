//
//  XYZ.h
//  S3A_renderer_dsp
//
//  Created by dylan on 09/12/2014.
//  Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
//

#ifndef __S3A_renderer_dsp__XYZ__
#define __S3A_renderer_dsp__XYZ__

#include "defs.h"

#include <iostream>
#include <cmath>

class XYZ {
 private:

 public:
 /**
  * Default constructor.
  */
   XYZ()
    : x( 0.0f )
    , y( 0.0f )
    , z( 0.0f )
    , isAtInfinity( false )
   {}

  Afloat x, y, z;
  bool isAtInfinity; // eg for source at infinity.
    
  int set(Afloat X, Afloat Y, Afloat Z, bool atInfinity = false) {
    x = X; y = Y; z =Z;
    isAtInfinity = false;
    return 0;
  };
    
  Afloat getLength() const {
    return std::sqrt(x*x + y*y + z*z);
  }    
};




#endif /* defined(__S3A_renderer_dsp__XYZ__) */
