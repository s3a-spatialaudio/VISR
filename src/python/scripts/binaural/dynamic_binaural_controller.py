# -*- coding: utf-8 -*-
"""
Copyright Institute of Sound and Vibration research - All rights reserved

S3A Binaural toolbox for the VISR framework

Created on Wed Sep  6 22:02:40 2017

@author: Andreas Franck a.franck@soton.ac.uk 
"""

from readSofa import sph2cart
from rotationFunctions import calcRotationMatrix

import visr
import pml
import rbbl
import objectmodel as om

import numpy as np
from scipy.spatial import KDTree

class DynamicBinauralController( visr.AtomicComponent ):
    """ Component to translate an object vector (and optionally head tracking information)
        into parameter parameters for dynamic binaural signal processing. """
    def __init__( self,
                  context, name, parent,    # Standard visr component constructor arguments
                  numberOfObjects,          # The number of point source objects rendered.
                  hrirPositions,            # The directions of the HRTF measurements, given as a Nx3 array
                  hrirData,                 # The HRTF data as 3 Nx2xL matrix, with L as the FIR length.
                  headRadius = 0.0875,      # Head radius, optional. Might be used in a dynamic ITD/ILD individualisation algorithm.
                  useHeadTracking = False,  # Whether head tracking data is provided via a self.headOrientation port.
                  dynamicITD = False,       # Whether ITD delays are calculated and sent via a "delays" port.
                  dynamicILD = False,       # Whether ILD gains are calculated and sent via a "gains" port.
                  hrirInterpolation = False, # HRTF interpolation selection: False: Nearest neighbour, True: Barycentric (3-point) interpolation
                  channelAllocation = False  # Whether to allocate object channels dynamically
                  ):
        # Call base class (AtomicComponent) constructor
        super( DynamicBinauralController, self ).__init__( context, name, parent )
        self.numberOfObjects = numberOfObjects
        
        # %% Define parameter ports
        self.objectInput = visr.ParameterInput( "objectVector", self, pml.ObjectVector.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )
        self.objectInputProtocol = self.objectInput.protocolInput()
        
        if useHeadTracking:
            self.useHeadTracking = True
            self.trackingInput = visr.ParameterInput( "headTracking", self, pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )
            self.trackingInputProtocol = self.trackingInput.protocolInput()

        else:
            self.useHeadTracking = False
            self.trackingInputProtocol = None # Flag that head tracking is not used.
            
        self.filterOutput = visr.ParameterOutput( "filterOutput", self,
                                                pml.IndexedVectorFloat.staticType,
                                                pml.MessageQueueProtocol.staticType,
                                                pml.EmptyParameterConfig() )
        self.filterOutputProtocol = self.filterOutput.protocolOutput()
        
        if dynamicITD:
            self.delayOutput = visr.ParameterOutput( "delayOutput", self,
                                                pml.VectorParameterFloat.staticType,
                                                pml.DoubleBufferingProtocol.staticType,
                                                pml.VectorParameterConfig( 2*self.numberOfObjects) )
            self.delayOutputProtocol = self.delayOutput.protocolOutput()
        else:
            self.delayOutputProtocol = None

        # We are always using the gain oputput matrix to set set the level, even if we do not use a loudness model.
        self.gainOutput = visr.ParameterOutput( "gainOutput", self,
                                               pml.VectorParameterFloat.staticType,
                                               pml.DoubleBufferingProtocol.staticType,
                                               pml.VectorParameterConfig( 2*self.numberOfObjects) )
        self.gainOutputProtocol = self.gainOutput.protocolOutput()

        if channelAllocation:
            self.routingOutput = visr.ParameterOutput( "routingOutput", self,
                                                     pml.SignalRoutingParameter.staticType,
                                                     pml.DoubleBufferingProtocol.staticType,
                                                     pml.EmptyParameterConfig() )
            self.routingOutputProtocol = self.routingOutput.protocolOutput()
        else:
            self.routingOutputProtocol = None

        # HRIR selection and interpolation data
        self.hrirs = np.array( hrirData, copy = True, dtype = np.float32 )
        
        # Normalise the hrir positions to unit radius (to let the k-d tree 
        # lookup work as expected.)
        hrirPositions[:,2] = 1.0
        self.hrirPos = sph2cart(np.array( hrirPositions, copy = True, dtype = np.float32 ))
        self.hrirInterpolation = hrirInterpolation
        if self.hrirInterpolation:
            self.lastPosition = np.repeat( [[np.NaN, np.NaN, np.NaN]], self.numberOfObjects, axis=0 )
        else:
            self.lastFilters = np.repeat( -1, self.numberOfObjects, axis=0 )
        self.hrirLookup = KDTree( self.hrirPos )
        
        # %% Dynamic allocation of objects to channels
        if channelAllocation:
            self.channelAllocator = rbbl.ObjectChannelAllocator( self.numberOfObjects )
            self.usedChannels = set()
        else:
            self.channelAllocator = None
            self.sourcePos = np.repeat( np.array([[1.0,0.0,0.0]]), self.numberOfObjects, axis = 0 )
            self.levels = np.zeros( (self.numberOfObjects), dtype = np.float32 )
        
    def process( self ):
        if self.objectInputProtocol.changed():
            ov = self.objectInputProtocol.data();

            if self.useHeadTracking:
                 if self.trackingInputProtocol.changed():
                     htrack = self.trackingInputProtocol.data()
                     ypr = htrack.orientation
                     rotationMatrix = calcRotationMatrix(np.array(ypr))
                     print(rotationMatrix)

            objIndicesRaw = [x.objectId for x in ov
                          if isinstance( x, (om.PointSource, om.PlaneWave) ) ]
            if self.channelAllocator is not None:
                self.channelAllocator.setObjects( objIndicesRaw )
                objIndices = self.channelAllocator.getObjectChannels()
                numObjects = len(objIndices)
                self.sourcePos = np.zeros( (numObjects,3), dtype=np.float32 )
                self.levels = np.zeros( (numObjects), dtype=np.float32 )
                
                for chIdx in range(0, numObjects):
                    objIdx = objIndices[chIdx]
                    self.sourcePos[chIdx,:] = ov[objIdx].position
                    self.levels[chIdx] = ov[objIdx].level
            else:
                self.levels[:] = 0.0
                for src in ov:
                    pos = np.asarray(src.position, dtype=np.float32 )
                    posNormed = 1.0/np.sqrt(np.sum(np.square(pos))) * pos
                    ch = src.channels[0]
                    self.sourcePos[ch,:] = posNormed
                    self.levels[ch] = src.level
                               
            if self.hrirInterpolation:
                [ d,indices ] = self.hrirLookup.query( self.sourcePos, 3, p =2 )
            else:
                [ d,indices ] = self.hrirLookup.query( self.sourcePos, 1, p =2 )

            # Retrieve the output gain vector for setting the object level and potentially
            # applying dynamically computed 
            gainVec = self.gainOutputProtocol.data()
            for chIdx in range(0,self.numberOfObjects):
                gain = self.levels[chIdx]
                # Set the object for both ears.
                # Note: Incorporate dynamically computed ILD is selected.
                gainVec[chIdx] = gain
                gainVec[chIdx+self.numberOfObjects] = gain
            
                # If the source is silent (probably inactive), don't change filters
                if gain >= 1.0e-7:
                    if self.hrirInterpolation:
                        raise ValueError( 'HRIR interpolation not implemented yet' )
                    else:
                        if self.lastFilters[chIdx] != indices[chIdx]:
                            leftCmd  = pml.IndexedVectorFloat( chIdx,
                                                              self.hrirs[indices[chIdx],0,:])
                            rightCmd = pml.IndexedVectorFloat( chIdx+self.numberOfObjects,
                                                              self.hrirs[indices[chIdx],1,:])
                            self.filterOutputProtocol.enqueue( leftCmd )
                            self.filterOutputProtocol.enqueue( rightCmd )
                            self.lastFilters[chIdx] = indices[chIdx]
            
            self.gainOutputProtocol.swapBuffers()
            self.objectInputProtocol.resetChanged()
            if self.useHeadTracking:
                self.trackingInputProtocol.resetChanged()
