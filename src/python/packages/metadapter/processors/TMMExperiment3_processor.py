# -*- coding: utf-8 -*-
"""
Created on Thu Mar 16 2017

@author: Jon Francombe
"""

import math

from metadapter import SequenceProcessorInterface

def dB2lin( dbVal ):
    return math.pow( 10.0, 0.05 * dbVal )

class TMMExperiment3Processor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)

        # Set default levels
        self.object1level = 0  # dB
        self.object2level = 0  # dB
        self.object3level = 0  # dB

        # Set default mutes
        self.object1mute = 0
        self.object2mute = 0
        self.object3mute = 0



    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.



        for obj in objectVector:

            objid = int(obj['id'])

            if objid == 6:
                obj['level'] = dB2lin(self.object1level)

                if self.object1mute:
                    obj['level'] = dB2lin(-120.0)

            elif objid == 7:
                obj['level'] = dB2lin(self.object2level)

                if self.object2mute:
                    obj['level'] = dB2lin(-120.0)

            elif objid == 8:
                obj['level'] = dB2lin(self.object3level)

                if self.object3mute:
                    obj['level'] = dB2lin(-120.0)

        return objectVector




    def setParameter( self, key, valueList ):

        if key == "object1level":
            self.object1level = valueList[0]

        elif key == "object2level":
            self.object2level = valueList[0]

        elif key == "object3level":
            self.object3level = valueList[0]

        elif key == "object1mute":
            self.object1mute = valueList[0]

        elif key == "object2mute":
            self.object2mute = valueList[0]

        elif key == "object3mute":
            self.object3mute = valueList[0]


        else:
            raise KeyError( "TMMExperiment3Processor: parameter not recognised")
