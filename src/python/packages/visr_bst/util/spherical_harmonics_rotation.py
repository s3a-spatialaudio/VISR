# -*- coding: utf-8 -*-
"""
Created on Sat Mar 17 16:59:20 2018

@author: af5u13
"""

import numpy as np

def allSphericalHarmonicsRotationMatrices( order, R ):
    allRs = [ np.asarray([1.0], dtype=R.dtype), R ]
    for currOrder in range(2, order+1 ):
        allRs.append( sphericalHarmonicsRotationMatrix( currOrder, R, allRs[-1] ) )
    return allRs



def sphericalHarmonicsRotationMatrix( order, R1, Rminus1 = None ):
    """
    Calculate a rotation matrix for sperical harmonics (SH) signals or coefficients of a given order.

    Algorithm: [1] Ivanic, J. & Ruedenberg, K. "Rotation Matrices for Real
    Spherical Harmonics. Direct Determination by Recursion"
    J. Phys. Chem, 1996, 100, 6342-6347, DOI 10.1021/jp953350u

    Parameters
    ----------
    order: int
        The spherical harmonic order, order >= 0
    R1: nump.ndarray
        The desired rotation as a rotation matrix. This is identical to the SH rotaton matrix for order 1.
        Dimension: 3x3
    Rminus1: numpy.ndarray, optional
        The SH rotation matrix for order order -1. Dimension (2*order-1)x(2*order-1).
        Optional argument, if not given, this value is computed recursively.
        This is more costly if the lower orders are required anyway.
    Returns
    -------
    numpy.ndarray
        Rotation matrix for the SHs of order 'order', dimension (2*order+1)x(2*order+1)
    """
    if order == 0:
        return np.asarray( [1,0], dtype = R1.dtype )
    if order == 1:
        return R1
    if Rminus1 is None:
        Rminus1 = sphericalHarmonicsRotationMatrix( order-1, R1, None )

    Pmtx = _calcPmatrix( order, R1, Rminus1 )

    uMatrix = _calcUmatrix( order, Pmtx )
    vMatrix = _calcVmatrix( order, Pmtx )
    wMatrix = _calcWmatrix( order, Pmtx )

    uCoeffs = _calcUcoeffs( order )
    vCoeffs = _calcVcoeffs( order )
    wCoeffs = _calcWcoeffs( order )

    res = uCoeffs * uMatrix + vCoeffs * vMatrix + wCoeffs * wMatrix
    return res

def _calcPmatrix( order, R1, Rminus1 ):
    """
    Calculate the quantity 'P' in Table 2 (lower half) of [1].

    This is a 3x(2*order+1)x(2*order+1) matrix, with dimension 0 correspnding to l,
    dimension 1 to mu (or m), and  dim. 2 to m'
    Note that the first and last element in dimension 1 (mu or m) is zeros, so
    it would not be technically necessary.
    The indices of dimension 0 are mapped to
    The SH degree of dim. 1 and to is mapped to an index as idx = m + order.
    """
    P = np.zeros( (3, 2*order+1, 2*order+1), dtype = R1.dtype )
    # Case |m'| < l
    P[:,1:-1,1:-1] = R1[:,1,np.newaxis,np.newaxis] * Rminus1[np.newaxis,:,:]
    # Case m' = l
    P[:,1:-1,-1] = (R1[:,-1,np.newaxis] * Rminus1[np.newaxis,:,-1]
                 - R1[:,0,np.newaxis] * Rminus1[np.newaxis,:,0])
    # Case m = -l
    P[:,1:-1,0] = (R1[:,-1,np.newaxis] * Rminus1[np.newaxis,:,0]
                 + R1[:,0,np.newaxis] * Rminus1[np.newaxis,:,-1])
    return P

def _calcUmatrix( order, P ):
    """
    Calculate the 'U' quantity of Table 1 in [1] as a (2*order+1), (2*order+1) matrix.
    First and last row are always zero.
    """
    return P[1,:,:]

def _calcVmatrix( order, P ):
    Vmtx = np.zeros( (2*order+1,2*order+1), dtype=P.dtype)
    # index  for degree m is (order+m)
    # degrees m = -order .. -2
    Vmtx[order-2::-1,:] = P[-1,order-1:0:-1,:] + P[0,order+1:-1,:]
    # degree m = -1
    Vmtx[order-1,:] = np.sqrt(2.0) * P[0,order,:]
    # degree m = 0
    Vmtx[order,:] = P[-1,order+1,:] + P[0,order-1,:]
    # degree m = 1
    Vmtx[order+1,:] = np.sqrt(2.0) * P[-1,order,:]
    # degrees m = 2 .. order
    Vmtx[order+2:,:] = P[-1,order+1:-1,:] - P[0,order-1:0:-1,:]

    return Vmtx

def _calcWmatrix( order, P ): # , R1, Rminus1 ):
    Wmtx = np.zeros( (2*order+1,2*order+1), dtype=P.dtype)
    # Note: the formula in the paper goes up to 'order', but the lat row would exceeed the size of P.
    # But since the coeff for this order would be zero anyway, we can leave that out.
    Wmtx[order-1:0:-1,:] = P[-1,order-2::-1,:] - P[0,order+2:,:]
    # Row m = 0 (row index 'order' remains zero )
    Wmtx[order+1:-1,:] =  P[-1,order+2:,:] + P[0,order-2::-1,:]
    return Wmtx

def _calcUcoeffs( order, dtype = np.float64 ):
    mRange = np.arange( -order, order+1, dtype=dtype )
    num = (order+mRange)*(order-mRange)
    mPrimeRange = np.arange( -order+1, order, dtype=dtype )
    den = np.array( np.concatenate( ( [2*order*(2*order-1)],
                                     (order+mPrimeRange)*(order-mPrimeRange),
                                     [2*order*(2*order-1)])
        ), dtype = dtype )
    uCoeffs = np.sqrt( num[:,np.newaxis] / den[np.newaxis,:] )
    return uCoeffs

def _calcVcoeffs( order, dtype = np.float64 ):
    mRange = np.arange( -order, order+1, dtype=dtype )
    # The application of the delta function is deferred to later.
    num = (order+np.abs(mRange)-1.0)*(order+np.abs(mRange))
    mPrimeRange = np.arange( -order+1, order, dtype=dtype )
    den = np.array( np.concatenate( ( [2*order*(2*order-1)],
                                     (order+mPrimeRange)*(order-mPrimeRange),
                                     [2*order*(2*order-1)])
        ), dtype = dtype )
    vCoeffs = 0.5 * np.sqrt( num[:,np.newaxis] / den[np.newaxis,:] )
    vCoeffs[order,:] *= -np.sqrt(2.0) # This corresponds to the two delta functions
    return vCoeffs

def _calcWcoeffs( order, dtype = np.float64 ):
    """
    """
    mRange = np.arange( -order, order+1, dtype=dtype )
    # The application of the delta function is deferred to later.
    num = (order-np.abs(mRange)-1.0)*(order-np.abs(mRange))
    mPrimeRange = np.arange( -order+1, order, dtype=dtype )
    den = np.array( np.concatenate( ( [2*order*(2*order-1)],
                                     (order+mPrimeRange)*(order-mPrimeRange),
                                     [2*order*(2*order-1)])
        ), dtype = dtype )
    wCoeffs = -0.5 * np.sqrt( num[:,np.newaxis] / den[np.newaxis,:] )
    wCoeffs[order,:] = 0.0 # This corresponds to the delta function
    return wCoeffs

if __name__ == "__main__":
    from visr_bst.util.rotation_functions import deg2rad, calcRotationMatrix, HOARotationMatrixCalc
    # Testing of internal function only
    from visr_bst.util.rotation_functions import P, uvw, U, V, W
    from time import time

    yaw = deg2rad( 30 )
    pitch = deg2rad( 45 )
    roll = deg2rad( 0.0 )

    R1 = calcRotationMatrix( np.asarray( (yaw,pitch,roll), dtype=np.float64 ) )

    maxOrder = 25
    rotMtxRef = [np.array([1.0]), R1]
    rotMatrices = [np.array([1.0]), R1]
    for nextOrd in range( 2, maxOrder ):
        Rn = sphericalHarmonicsRotationMatrix( nextOrd, rotMatrices[1], rotMatrices[-1] )
        rotMatrices.append( Rn )
        RnR = HOARotationMatrixCalc( nextOrd, rotMtxRef[-1],  rotMtxRef[1] )
        rotMtxRef.append( RnR )

    for i in range(maxOrder):
        print( "order: %d error: %f " % (i, np.max(np.abs( rotMatrices[i]-rotMtxRef[i] )) ) )

    # Timing loop
    numIters = 500
    maxOrder = 25

    for order in range( 2, maxOrder+1):
        Rminus1 = sphericalHarmonicsRotationMatrix( order-1, R1 )

        tStart = time()
        for i in range(numIters):
            Rn = sphericalHarmonicsRotationMatrix( order, R1, Rminus1 )
        tElapsedNew = time() - tStart

        tStart = time()
        for i in range(numIters):
            Rn = HOARotationMatrixCalc( order, Rminus1, R1 )
        tElapsedOld = time() - tStart

        print( "Time for SH rotation matrix of order %i: Old: %f ms, New: %f ms, Speedup %f."
              % (order, 1000.0/ numIters * tElapsedOld, 1000.0/ numIters * tElapsedNew, tElapsedOld / tElapsedNew ) )

    # Test the all-in one function

    allRs = allSphericalHarmonicsRotationMatrices( 30, R1 )


    # %% Detailed test of internals
    order = 2
    Pref = np.zeros( (3, 2*order-1, 2*order+1), dtype = np.float64 )
    for i in range(-1, 1+1):
        for mu in range(-order+1, order+1-1):
            for mprime in range( -order, order+1):
                #  P(i, l, m, n, R_lm1, R_1):
                Pref[i+1,mu+order-1,mprime+order] = P( i, order, mu, mprime, R1, R1 )

    Pnew = _calcPmatrix( order, R1, R1 )

    # Check the u,v, and W coeffs
    uCoeffRef = np.zeros( (2*order+1, 2*order+1), dtype=np.float64 )
    vCoeffRef = np.zeros( (2*order+1, 2*order+1), dtype=np.float64 )
    wCoeffRef = np.zeros( (2*order+1, 2*order+1), dtype=np.float64 )

    uMtxRef = np.zeros( (2*order+1, 2*order+1), dtype=np.float64 )
    vMtxRef = np.zeros( (2*order+1, 2*order+1), dtype=np.float64 )
#    wMtxRef = np.zeros( (2*order+1, 2*order+1), dtype=np.float64 )

    for m in range(-order, order+1):
        for mPrime in range( -order, order+1):
            u,v,w = uvw( order, m, mPrime )
            uCoeffRef[m+order,mPrime + order ] = u
            vCoeffRef[m+order,mPrime + order ] = v
            wCoeffRef[m+order,mPrime + order ] = w
            if np.abs(m) < order:
                uMtxRef[m+order,mPrime + order ] = U( order, m, mPrime, R1, R1 )
            vMtxRef[m+order,mPrime + order ] = V( order, m, mPrime, R1, R1 )
#            if np.abs(m) < order:
#                wMtxRef[m+order,mPrime + order ] = W( order, m, mPrime, R1, R1 )

    uCoeffs = _calcUcoeffs( order )
    vCoeffs = _calcVcoeffs( order )
    wCoeffs = _calcWcoeffs( order )

    Umtx = _calcUmatrix( order, Pnew )
    Vmtx = _calcVmatrix( order, Pnew )
    Wmtx = _calcWmatrix( order, Pnew )

