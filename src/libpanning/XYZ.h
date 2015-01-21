//
//  XYZ.h
//
//  Created by Dylan Menzies on 09/12/2014.
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
    , isInfinite( false )
   {}
 
  /**
   * Constructor with specified coordinates.
   */
  explicit XYZ( Afloat pX, Afloat pY, Afloat pZ, bool pInfinity = false )
    : x( pX), y( pY ), z( pZ ), isInfinite( pInfinity )
  {}
 
  /**
   * Assignment operator. Uses default-constructed operator (no implementation required).
   */
  XYZ & operator=( XYZ const & rhs ) = default;
 
  /**
   * Copy constructor.
   * Uses automatically-generated default copy ctor, no implementation required.
   */
  XYZ( XYZ const & rhs ) = default;
  
  Afloat x, y, z;
  bool isInfinite; // eg for source at infinity.
    
  int set(Afloat X, Afloat Y, Afloat Z, bool inf = false) {
    x = X; y = Y; z =Z;
    isInfinite = inf;
    return 0;
  };
    
  Afloat getLength() const {
    return std::sqrt(x*x + y*y + z*z);
  }    
};




#endif /* defined(__S3A_renderer_dsp__XYZ__) */
