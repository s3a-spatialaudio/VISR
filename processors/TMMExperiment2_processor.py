# -*- coding: utf-8 -*-
"""
Created on Thu Mar 16 2017

@author: Jon Francombe
"""

import math

from metadapter import SequenceProcessorInterface

def dB2lin( dbVal ):
    return math.pow( 10.0, 0.05 * dbVal )

class TMMExperiment2Processor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)

        # Set defaults
        self.source2az = 0     # degrees
        self.source2level = 0  # dB
        self.noiseon = 0    # flag


    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.



        for obj in objectVector:

            objid = int(obj['id'])

            if objid == 1:                                  # Source 2 (can be moved by the user and the level is changed in the experiment design)
                obj['position']['az'] = self.source2az
                obj['level'] = dB2lin(self.source2level)
            elif objid == 2:                                # Source 3 (noise) — level is set in the experiment design

                if self.noiseon:
                	obj['level'] = dB2lin(0)
                else:
                	obj['level'] = 0.0

        return objectVector




    def setParameter( self, key, valueList ):

        if key == "noiseon":
            self.noiseon = valueList[0]

        elif key == "source2az":
            self.source2az = valueList[0]

        elif key == "source2level":
            self.source2level = valueList[0]


        else:
            raise KeyError( "TMMExperiment2Processor: parameter not recognised")
