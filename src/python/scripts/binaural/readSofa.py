# -*- coding: utf-8 -*-
"""
Created on Wed Sep  6 22:02:40 2017

@author: af5u13
"""

import os
import numpy as np
import h5py
from scipy.spatial import KDTree

def sph2cart( sph ):
    elFactor = np.cos( sph[:,1] )
    x = np.cos( sph[:,1] ) * elFactor * sph[:,2] 
    y = np.sin( sph[:,1] ) * elFactor * sph[:,2]
    z = np.sin( sph[:,1] ) * sph[:,2]
    cart = np.stack( (x,y,z), 1 )
    return cart
    
def deg2rad( phi ):
    return (np.pi/180.0) * phi

def convertSofaSphToSph( sofaPos ):
    az = deg2rad( sofaPos[:,0] )
    el = deg2rad( sofaPos[:,1] )
    
    pos = np.stack( (az,el, sofaPos[:,2] ), 1 )
    
    return pos

def readSofaFile( fileName, dtype = np.float32 ):
    if not os.path.exists( fileName ):
        raise ValueError( "SOFA file does not exist." )
    h = h5py.File( fileName )
    
    sofaPos = np.asarray( h.get('SourcePosition'), dtype=dtype )
    
    pos = convertSofaSphToSph( sofaPos )
    
    hrir = np.asarray( h.get('Data.IR'), dtype=dtype )
    
    return pos, hrir

# Example code:
if __name__ == '__main__':
    filePath = 'c:/local/s3a_af/subprojects/binaural/hrtf b_nh169.sofa'
    
    posSph, hrir = readSofaFile( filePath )
    posSph[:,2] = 1 # Normalise to unit radius
    
    # 3D positions in the renderer coordinate system.
    pos = sph2cart( posSph )

    # Construct a KD tree to enable fast nearest-neighbout and interpolant 
    kd = KDTree( pos )

