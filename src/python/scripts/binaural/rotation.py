#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Sep 12 10:28:06 2017

@author: gc1y17
"""

import os
import numpy as np


def sph2cart( sph ):
    elFactor = np.cos( sph[:,1] )
    x = np.cos( sph[:,1] ) * elFactor * sph[:,2] 
    y = np.sin( sph[:,1] ) * elFactor * sph[:,2]
    z = np.sin( sph[:,1] ) * sph[:,2]
    cart = np.stack( (x,y,z), 1 )
    return cart
    
def deg2rad( phi ):
    return (np.pi/180.0) * phi


