# -*- coding: utf-8 -*-
"""

Spherical harmonic functions for HOA synthesis


Created on Thu Oct  5 11:24:11 2017

@author: af5u13
"""

import numpy as np
from math import factorial, sqrt
import scipy

def sphHarmReal( n, m, theta, phi, dtype = None ):
    """
    Real spherical harmonics function.
    """ 
    def trig( m, phi, dtype ):
        if m == 0:
            return np.ones( phi.shape, dtype = dtype )
        elif m < 0:
            return np.sin( phi * abs(m) ) * sqrt(2.0)
        else:
            return np.cos( phi * m ) * sqrt(2.0)
 
    if dtype is None:
        dtype = np.asarray(theta).dtype
    
    (thetaBC, phiBC) = np.broadcast_arrays( theta, phi )
    mabs = abs(m)
    
    fac = sqrt( ((2*n+1)*factorial(n-mabs)) /(4*np.pi*factorial(n+mabs)))
    # Apparently we need to compensate for the Condon-Shortley phase
    # Although the scipy documentation says 
    # "Note that this implementation includes the Condon-Shortley phase."
    lp = fac * (-1)**mabs * scipy.special.lpmv(  mabs, n, np.cos(thetaBC) )
    return trig( m, phiBC, dtype ) * lp 

def allSphHarmRealACN( N, theta, phi, dtype = None ):
    numCoeffs = (N+1)*(N+1)
    [thetaBC, phiBC ] = np.broadcast_arrays( theta, phi )    
    argShape = thetaBC.shape
    if dtype is None:
        dtype = thetaBC.dtype
    Y = np.NaN * np.ones( (numCoeffs,)+argShape, dtype = dtype )
    for n in range(0,N+1):
        for m in range(-n,n+1):
            Y[n**2+n+m,...] = sphHarmReal( n, m, theta, phi, dtype )
    return Y
