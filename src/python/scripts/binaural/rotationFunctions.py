#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Sep 15 16:50:56 2017

@author: gc1y17
"""
import math
import numpy as np

# note: the input format is inconsistent with the return value and with sph2cart
# (individual values vs single matrix)
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
    elFactor = np.cos( sph[...,1] )
    x = np.cos( sph[...,0] ) * elFactor * sph[...,2]
    y = np.sin( sph[...,0] ) * elFactor * sph[...,2]
    z = np.sin( sph[...,1] ) * sph[...,2]
    cart = np.stack( (x,y,z), axis=-1 )
    return cart

def sph2cart3inp(az,el,r):
    x = r*np.cos(az)*np.cos(el)
    y = r*np.sin(az)*np.cos(el)
    z = r*np.sin(el)
    return np.stack((x,y,z),0)

def calcRotationMatrix(ypr):
  if ypr.size == 3 :

#    psi = deg2rad(np.float32(ypr.item(0)))
    phi = np.float32(ypr.item(0))

#    the = deg2rad(np.float32(ypr.item(1)))
    the = np.float32(ypr.item(1))

#    phi = deg2rad(np.float32(ypr.item(2)))
    psi = np.float32(ypr.item(2))

    a11 = math.cos(the) * math.cos(phi)
    a12 = math.cos(the) * math.sin(phi)
    a13 = -math.sin(the)

    a21 = math.sin(psi) * math.sin(the) * math.cos(phi) - math.cos(psi) * math.sin(phi)
    a22 = math.sin(psi) * math.sin(the) * math.sin(phi) + math.cos(psi) * math.cos(phi)
    a23 = math.cos(the) * math.sin(psi)

    a31 = math.cos(psi) * math.sin(the) * math.cos(phi) + math.sin(psi) * math.sin(phi)
    a32 = math.cos(psi) * math.sin(the) * math.sin(phi) - math.sin(psi) * math.cos(phi)
    a33 = math.cos(the) * math.cos(psi)

    rotation = np.array([[a11, a12, a13], [a21, a22, a23], [a31, a32, a33]])
#    print(rotation)
    return rotation
  else:
    return np.identity(3)

def delta(m, n):
	# Kronecker Delta
    return 1 if m == n else 0

def P(i, l, m, n, R_lm1, R_1):
    # all the indices of the formulation are translated from bipolar to positive
#    print(" bef i%d l%d [%d,%d]"%(i,l,m,n))
    adj=1 #just to make clear it is an adjustment for positive indices
    i+=adj
    sz = l-1
    m+=sz

    if n == -l : # originally m == -l
        return  R_1[i,1+adj] * R_lm1[m,n+sz+1] \
              + R_1[i,-1+adj] * R_lm1[m,-n+sz-1]
    elif n == l :
        return  R_1[i,1+adj] * R_lm1[m,n+sz-1] \
              - R_1[i,-1+adj] * R_lm1[m,-n+sz+1]
    else :# 0<m<l
        return R_1[i,0+adj] * R_lm1[m,n+sz]

def U(l, m, n, R_lm1, R_1):
#    print("U %d [%d,%d]"%(l,m,n))
    return (P(0, l, m, n, R_lm1, R_1))

def V(l, m, n, R_lm1, R_1):
#    print("V %d [%d,%d]"%(l,m,n))
    if m == 0 :
        return  P(1, l, 1, n, R_lm1, R_1) \
              + P(-1, l, -1, n, R_lm1, R_1)
    elif m > 0 :
        d = delta(m, 1)
        return  P(1, l, m - 1, n, R_lm1, R_1) * math.sqrt(1+d) \
              - P(-1, l, -m + 1, n, R_lm1, R_1) * (1-d)
    else : # m < 0
        d = delta(m, -1)
        return  P(1, l, m + 1, n, R_lm1, R_1) * (1 - d) \
              + P(-1, l, -m - 1, n, R_lm1, R_1) * math.sqrt(1 + d)

def W(l, m, n, R_lm1, R_1):
#    print("W %d [%d,%d]"%(l,m,n))
    if m == 0 :
        # Never gets called as kd=0
#        ASSERT(false);
        return (0);
    elif m > 0 :
        return  P(1, l, m + 1, n, R_lm1, R_1) \
              + P(-1, l, -m - 1, n, R_lm1, R_1)
    else : # m < 0
        return  P(1, l, m - 1, n, R_lm1, R_1) \
              - P(-1, l, -m + 1, n, R_lm1, R_1)

def uvw(l, m, n):
    d = delta(m, 0)
    _m = abs(m)
    den = (2 * l) * (2 * l - 1) if abs(n) == l else (l + n) * (l - n)

    u = math.sqrt((l + m) * (l - m) / den)
    v = 0.5 * math.sqrt((1 + d) * (l + _m - 1) * (l + _m) / den) * (1 - 2 * d)
    w = -0.5 * math.sqrt((l - _m - 1) * (l - _m) / den) * (1 - d)
    return u,v,w


def Rmatrix(l, m, n, R_lm1, R_1):
    u,v,w  = uvw(l,m,n)

    if u:
        u *= U(l, m, n, R_lm1, R_1)
    if v:
        v *= V(l, m, n, R_lm1, R_1)
    if w:
        w *= W(l, m, n, R_lm1, R_1)

    return u+v+w


def HOARotationMatrixCalc(l, R_lm1, R_1):
    size = 2*l+1
#    print(R_1)
    r = np.zeros( (size,size), dtype=np.float32 )
# the correct spherical harmonics indices should span from -l to l (included),
# here in the output matrix it is translated by l, to have only positive indices
    for m in range(0,size) :
        for n in range(0,size) :
#            print("R [%d,%d]"%(m-l, n-l))
            r[m][n] = Rmatrix(l, m-l, n-l, R_lm1, R_1);

    return r


def rotationMatrixReorderingACN(r):
    #a permutation of the 3x3 rotation matrix is done to move rows and columns from xyz ordering to yzx,
    # which is the spherical harmonics ACN ordering
    perm = [1,2,0]
    r = r[:,perm]
    r = r[perm,:]
    return r

##testing
#np.set_printoptions(linewidth=10000)
#np.set_printoptions(threshold=np.nan)
#R_1 = calcRotationMatrix(np.array([np.pi/2,0,0]))
#R_1 = rotationMatrixReorderingACN(R_1)
#
#print(R_1)
#print()
#
#R_2 = HOARotationMatrixCalc(2,R_1,R_1)
#print(R_2)
#print()
#
#R_3 = HOARotationMatrixCalc(3,R_2,R_1)
#print(R_3)
#print()
