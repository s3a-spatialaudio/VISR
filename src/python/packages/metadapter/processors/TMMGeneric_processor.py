# -*- coding: utf-8 -*-
"""
Created on Thu Mar 16 2017

@author: Jon Francombe
"""

import math

from metadapter import SequenceProcessorInterface

def dB2lin( dbVal ):
    return math.pow( 10.0, 0.05 * dbVal )

class TMMGenericProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)

        # Set defaults
        self.az0 = 0      
        self.az1 = 0
        self.az2 = 0
        self.az3 = 0
        self.az4 = 0
        self.az5 = 0
        self.az6 = 0
        self.az7 = 0
                	
        self.lev0 = 0        
        self.lev1 = 0
        self.lev2 = 0
        self.lev3 = 0
        self.lev4 = 0
        self.lev5 = 0
        self.lev6 = 0
        self.lev7 = 0
        
        self.overall_level = 0

    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.



        for obj in objectVector:

            objid = int(obj['id'])

            if objid == 0:                                  
                obj['position']['az'] = self.az0
                obj['level'] = dB2lin(self.lev0)
                
            elif objid == 1:                                  
                obj['position']['az'] = self.az1
                obj['level'] = dB2lin(self.lev1)
            
            elif objid == 2:                                  
                obj['position']['az'] = self.az2
                obj['level'] = dB2lin(self.lev2)

            elif objid == 3:                                  
                obj['position']['az'] = self.az3
                obj['level'] = dB2lin(self.lev3)
            
            elif objid == 4:                                  
                obj['position']['az'] = self.az4
                obj['level'] = dB2lin(self.lev4)
                
            elif objid == 5:                                  
                obj['position']['az'] = self.az5
                obj['level'] = dB2lin(self.lev5)
            
            elif objid == 6:                                  
                obj['position']['az'] = self.az6
                obj['level'] = dB2lin(self.lev6)

            elif objid == 7:                                  
                obj['position']['az'] = self.az7
                obj['level'] = dB2lin(self.lev7)

            # Overall level on all objects
            obj['level'] = obj['level'] * dB2lin(self.overall_level)

        return objectVector



    
    def setParameter( self, key, valueList ):

        print('key: %s.    value: %f' % (key, valueList[0]) )  

        if key == "az0":
        	self.az0 = valueList[0]      
        elif key == "az1":
        	self.az1 = valueList[0]
        elif key == "az2":
        	self.az2 = valueList[0]
        elif key == "az3":
        	self.az3 = valueList[0]
        elif key == "az4":
        	self.az4 = valueList[0]
        elif key == "az5":
        	self.az5 = valueList[0]
        elif key == "az6":
        	self.az6 = valueList[0]
        elif key == "az7":
        	self.az7 = valueList[0]
        	
        elif key == "lev0":
        	self.lev0 = valueList[0]        
        elif key == "lev1":
        	self.lev1 = valueList[0]
        elif key == "lev2":
        	self.lev2 = valueList[0]
        elif key == "lev3":
        	self.lev3 = valueList[0]
        elif key == "lev4":
        	self.lev4 = valueList[0]
        elif key == "lev5":
        	self.lev5 = valueList[0]
        elif key == "lev6":
        	self.lev6 = valueList[0]
        elif key == "lev7":
        	self.lev7 = valueList[0]
        	
        elif key == "overall_level":
        	self.overall_level = valueList[0]
        	
        else:
            raise KeyError( "TMMGenericProcessor: parameter %s not recognised" % key)
