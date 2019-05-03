# -*- coding: utf-8 -*-
"""
Created on Thu Oct  5 12:51:14 2017

@author: af5u13
"""

from real_sph_harmonics import sphHarmReal, allSphHarmRealACN
from rotationFunctions import sph2cart3inp, cart2sph
import numpy as np
import matplotlib.pyplot as plt

phi = np.arange( 0, 2*np.pi, np.pi/180 )

theta = 0.1 * np.sin( phi )

# Desired direction ("velocity") vectors
Pdes = sph2cart3inp( phi, theta ).T

N = 3

Y = allSphHarmRealACN( N, np.pi/2-theta, phi )

#L = np.asarray( [ [1,0,-1,0,0,0], [0,1,0,-1,0,0], [0,0,0,0,1,-1] ], dtype = np.float32 )

# Load a regular array config (spherical t-design)
designFile = './data/des.3.24.7.txt'
# designFile = './data/des.3.70.11.txt'
#designFile = 'data/des.3.240.21.txt'
rawData = np.loadtxt( designFile, dtype = np.float32 )
L = np.transpose(np.reshape( rawData, [rawData.size // 3, 3 ]))
Laz, Lel, Lrad = cart2sph( L[0,:], L[1,:], L[2,:] )

Psi = allSphHarmRealACN( N, np.pi/2-Lel, Laz, dtype = np.float32 )

D = np.linalg.pinv(Psi)

g = np.matmul( D, Y )

# Check the resulting panned directions
P = np.matmul( L, g )
azAct, elAct, radAct = cart2sph( P[0,:], P[1,:], P[2,:] )

l2 = np.sqrt(np.sum( np.power( g, 2 ), 0 ))
l1 = np.sum( np.abs( g ), 0 )

Yact = np.matmul(Psi,g)
 

#y = sphHarmReal( 2, 2, theta, phi )

#plt.figure()
#plt.plot( phi, y, 'r.-' )
#
#plt.figure()
#plt.polar( phi, y, 'r.-' )
#plt.ylim(-1,1)


