# -*- coding: utf-8 -*-
"""
Created on Thu Mar 16 2017

@author: Jon Francombe
"""

import math

from metadapter import SequenceProcessorInterface

def dB2lin( dbVal ):
    return math.pow( 10.0, 0.05 * dbVal )

class TMMExperiment3_MUSHRAProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)

        # Set defaults
        self.pianolevel = 0  # dB
        self.pianoazimuth = 0  # deg
        self.overall_level = 0 # dB


    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.



        for obj in objectVector:

            objid = int(obj['id'])

            if objid == 5:                                  
                obj['level'] = dB2lin(self.pianolevel)
                
                obj['position']['az'] = self.pianoazimuth
                
            obj['level'] = obj['level'] * dB2lin(self.overall_level)
                

        return objectVector



    
    def setParameter( self, key, valueList ):

        if key == "pianolevel":
            self.pianolevel = valueList[0]

        elif key == "pianoazimuth":
            self.pianoazimuth = valueList[0]
            
        elif key == "overall_level":
            self.overall_level = valueList[0]
            print("Overall level: %f" % valueList[0])

        else:
            raise KeyError( "TMMExperiment3_MUSHRAProcessor: parameter not recognised")
