#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Sep 15 16:50:56 2017

@author: gc1y17
"""
import math
import numpy as np

idMatrix = np.identity(3)

def calcRotationMatrix(ypr):  
  if ypr.size == 3 :
   # print(type(ypr))
    psi = np.float32(ypr.item(0))
   # print(type(psi))
    the = np.float32(ypr.item(1))
   # print(type(the))
    phi = np.float32(ypr.item(2))

    a11 = math.cos(the) * math.cos(phi)
    a12 = np.math.cos(the) * np.math.sin(phi)
    a13 = -np.math.sin(the)
    
    a21 = np.math.sin(psi) * np.math.sin(the) * np.math.cos(phi) - np.math.cos(psi) * np.math.sin(phi)
    a22 = np.math.sin(psi) * np.math.sin(the) * np.math.sin(phi) + np.math.cos(psi) * np.math.cos(phi)
    a23 = np.math.cos(the) * np.math.sin(psi)

    a31 = np.math.cos(psi) * np.math.sin(the) * np.math.cos(phi) + np.math.sin(psi) * np.math.sin(phi)
    a32 = np.math.cos(psi) * np.math.sin(the) * np.math.sin(phi) - np.math.sin(psi) * np.math.cos(phi)
    a33 = np.math.cos(the) * np.math.cos(psi)
    
    rotation = np.matrix([[a11, a12, a13], [a21, a22, a23], [a31, a32, a33]])
    return rotation
  else:
    return idMatrix
 
    
def cart2sph(x,y,z):
    radius = math.sqrt( x*x + y*y + z*z );
    az = math.atan2( y, x );
    el = math.asin( z / radius );
    #print('%f %f %f'%(az,el,radius))
    sph = np.stack( (az, el, radius) )

    return sph;

def deg2rad( phi ):
    return (np.pi/180.0) * phi

def rad2deg( phi ):
    return (180.0/np.pi) * phi

def sph2cart( sph ):
    elFactor = np.cos( sph[:,1] )
    x = np.cos( sph[:,1] ) * elFactor * sph[:,2] 
    y = np.sin( sph[:,1] ) * elFactor * sph[:,2]
    z = np.sin( sph[:,1] ) * sph[:,2]
    cart = np.stack( (x,y,z), 1 )
    return cart
    