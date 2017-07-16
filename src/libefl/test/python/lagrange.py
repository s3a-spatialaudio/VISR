# -*- coding: utf-8 -*-
"""
Created on Sun May 28 09:02:02 2017

@author: andi
"""

def storageRequirements( N ):
    if N < 2:
        return N
    else:
        return N + storageRequirements( (N+1)//2 )