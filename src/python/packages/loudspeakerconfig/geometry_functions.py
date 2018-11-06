# -*- coding: utf-8 -*-
"""
Created on Thu Nov  1 10:59:01 2018

@author: af5u13
"""

import numpy as np

def deg2rad( phi ):
    return (np.pi/180.0) * phi

def rad2deg( phi ):
    return (180.0/np.pi) * phi

def cart2sph( X ):
    x = X[...,0]
    y = X[...,1]
    z = X[...,2]
    radius = np.sqrt( x*x + y*y + z*z );
    az = np.arctan2( y, x );
    el = np.arcsin( z / radius );
    return az, el, radius

def sph2cart( az, el, r ):
    (azBC, elBC, rBC ) = np.broadcast_arrays( az, el, r )
    elFactor = np.cos( elBC )
    x = np.cos( azBC ) * elFactor * rBC
    y = np.sin( azBC ) * elFactor * rBC
    z = np.sin( elBC ) * rBC
    cart = np.stack( (x,y,z), -1 )
    return cart
