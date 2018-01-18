# -*- coding: utf-8 -*-
"""
Created on Wed Sep  6 22:02:40 2017

@author: af5u13
"""

import os
import numpy as np
import h5py
from scipy.spatial import KDTree

from rotationFunctions import sph2cart

def deg2rad( phi ):
    return (np.pi/180.0) * phi

def convertSofaSphToSph( sofaPos ):
    az = deg2rad( sofaPos[:,0] )
    el = deg2rad( sofaPos[:,1] )

    pos = np.stack( (az,el, sofaPos[:,2] ), 1 )

    return pos

def readSofaFile( fileName, dtype = np.float32,
                 truncationLength = None,
                 truncationWindowLength = 0 ):
    if not os.path.exists( fileName ):
        raise ValueError( "SOFA file does not exist." )
    fileH = h5py.File( fileName, 'r' )
    try:
        sofaPos = np.asarray( fileH.get('SourcePosition'), dtype=dtype )

        pos = convertSofaSphToSph( sofaPos )

        hrir = np.asarray( fileH.get('Data.IR'), dtype=dtype )
        hrirLength = hrir.shape[-1]
        if (not truncationLength is None) and (truncationLength < hrirLength ):
            windowFcn = np.ones( (truncationLength), dtype=dtype )
            if truncationWindowLength > hrirLength:
                raise ValueError( "Transition window length exceeds HRIR length." )
            if truncationWindowLength > 0:
                fadeOut = 0.5*(np.cos( np.pi*np.arange(1.0,1.0+truncationWindowLength)/(2.0+truncationWindowLength)) + 1.0)
                windowFcn[-truncationWindowLength:] = fadeOut
            hrir = hrir[...,:truncationLength] * windowFcn

        rawDelays = np.asarray( fileH.get('Data.Delay'), dtype=dtype )
        if rawDelays.shape == hrir.shape[0:-1]: # Check whether delays are per measurement point
            fs = float( np.asarray( fileH.get('Data.SamplingRate')) )
            delays  = rawDelays / fs
        else:
            delays = None

        return pos, hrir, delays
    finally:
        fileH.close()

# Example code:
if __name__ == '__main__':
    currDir = os.getcwd()
    filePath = os.path.join( currDir, 'data/dtf b_nh169.sofa' )

    posSph, hrir, delays = readSofaFile( filePath )
    posSph[:,2] = 1 # Normalise to unit radius

    # 3D positions in the renderer coordinate system.
    pos = sph2cart( posSph )

    # Construct a KD tree to enable fast nearest-neighbour and interpolation
    kd = KDTree( pos )
