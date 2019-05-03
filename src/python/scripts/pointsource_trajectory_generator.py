# -*- coding: utf-8 -*-
"""
Created on Sun Feb 26 16:41:01 2017

@author: andi
"""

import visr
import panning
import pml
import objectmodel as om

import numpy as np
import matplotlib.pyplot as plt

class PointSourceTrajectoryGenerator( visr.AtomicComponent ):
    def __init__( self, context, name, parent,
                 positions, # Either #points x 3 or 1x3 matrix of Cartesian object positions.
                 updateRateSamples = None,
                 objectId = 0,
                 groupId = 0,
                 priority = 0,
                 objectLevel = 1.0,
                 objectChannel = None,
                 diffuseness = None ):
        super( PointSourceTrajectoryGenerator, self ).__init__( context, name, parent )
        if updateRateSamples % self.period() != 0:
            raise ValueError( "TrajectoryGenerator: The update rate must be a multiple of the period." )
        self.updateCycles = updateRateSamples // self.period()

        self.positions = positions
        self.numPositions = self.positions.shape[1]

        self.objectOutput = visr.ParameterOutput( "objectVectorOutput", self,
                                               parameterType=pml.ObjectVector.staticType,
                                               protocolType=pml.DoubleBufferingProtocol.staticType,
                                               parameterConfig=pml.EmptyParameterConfig() )
        self.cycleCounter = 0
        self.positionCounter = 0

        if diffuseness is None :
            self.object = om.PointSource( objectId )
        else:
            ValueError( "Diffuse point sources not currently supported.")

        self.object.position = positions[:,0]
        self.object.level = objectLevel
        self.groupId = groupId
        self.object.priority = priority
        if objectChannel is None:
            objectChannel = objectId
        self.object.channels = [objectChannel]

    def process( self ):
        if self.cycleCounter == 0:
            self.object.position = self.positions[:,self.positionCounter]
            op = self.objectOutput.protocolOutput()
            op.data().insert( self.object )
            op.swapBuffers()
            self.positionCounter = (self.positionCounter + 1 ) % self.numPositions
            # print( "Updated object position: %s", str(self.positions[:,self.positionCounter]) )
        self.cycleCounter = (self.cycleCounter+1) % self.updateCycles
