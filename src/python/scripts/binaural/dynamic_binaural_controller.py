# -*- coding: utf-8 -*-
"""
Copyright Institute of Sound and Vibration research - All rights reserved

S3A Binaural toolbox for the VISR framework

Created on Wed Sep  6 22:02:40 2017

@author: Andreas Franck a.franck@soton.ac.uk 
"""
from numpy.linalg import inv 
from readSofa import sph2cart
from rotationFunctions import calcRotationMatrix, cart2sph, rad2deg
import matplotlib.pyplot as plt
import visr
import pml
import rbbl
import objectmodel as om
import time
import cProfile, pstats, io
import numpy as np
import warnings

from scipy.spatial import ConvexHull

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
                  channelAllocation = False, # Whether to allocate object channels dynamically
                  delays = None,             # Matrix of delays associated with filter dataset. Dimension: # filters * 2
                  ):
        # Call base class (AtomicComponent) constructor
        super( DynamicBinauralController, self ).__init__( context, name, parent )
        self.numberOfObjects = numberOfObjects
        self.dynamicITD = dynamicITD
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
            
        self.delayOutput = visr.ParameterOutput( "delayOutput", self,
                                                pml.VectorParameterFloat.staticType,
                                                pml.DoubleBufferingProtocol.staticType,
                                                pml.VectorParameterConfig( 2*self.numberOfObjects) )
        self.delayOutputProtocol = self.delayOutput.protocolOutput()

        if self.dynamicITD:
            if (delays is None) or (delays.ndim != 2) or (delays.shape != (hrirData.shape[0], 2 ) ):
                raise ValueError( 'If the "dynamicITD" option is given, the parameter "delays" must be a #hrirs x 2 matrix.' )

            self.dynamicDelays = np.array(delays, copy=True)
        else:
            self.dynamicDelays = None

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
            self.hrirLookup = ConvexHull( self.hrirPos )
            self.triplets = np.transpose(self.hrirLookup.points[self.hrirLookup.simplices], axes=(0, 2, 1))
            self.inverted = inv(self.triplets)
        else:
            self.lastFilters = np.repeat( -1, self.numberOfObjects, axis=0 )

        # %% Dynamic allocation of objects to channels
        if channelAllocation:
            self.channelAllocator = rbbl.ObjectChannelAllocator( self.numberOfObjects )
            self.usedChannels = set()
        else:
            self.channelAllocator = None
            self.sourcePos = np.repeat( np.array([[1.0,0.0,0.0]]), self.numberOfObjects, axis = 0 )
            self.levels = np.zeros( (self.numberOfObjects), dtype = np.float32 )

#        self.f = open('srcpAllinone.txt', 'w')

    def process( self ):

##PROFILING                    
##        startTot = time.time()
#        pr = cProfile.Profile()
#        pr.enable()
        
        if self.objectInputProtocol.changed():
            ov = self.objectInputProtocol.data();
            objIndicesRaw = [x.objectId for x in ov
                          if isinstance( x, (om.PointSource, om.PlaneWave) ) ]
            self.levels[:] = 0.0
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
                for index,src in enumerate(ov):
                    if index < self.numberOfObjects :
                        pos = np.asarray(src.position, dtype=np.float32 )
                        posNormed = 1.0/np.sqrt(np.sum(np.square(pos))) * pos
                        ch = src.channels[0]
                        self.sourcePos[ch,:] = posNormed
                        self.levels[ch] = src.level
                    else:
                        warnings.warn('The number of dynamically instantiated sound objects is more than the maximum number specified')                            
                        break              

            # TODO: This belongs somewhere else in the recompute logic.
            if self.useHeadTracking:
                 if self.trackingInputProtocol.changed():
                     htrack = self.trackingInputProtocol.data()
                     ypr = htrack.orientation
                     
                     # np.negative is to obtain the opposite rotation of the head rotation, i.e. the inverse matrix of head rotation matrix
                     rotationMatrix = calcRotationMatrix(np.negative(ypr))
                     self.sourcePos = np.array(np.matmul(self.sourcePos,rotationMatrix))

            # Obtain access to the output arrays
            gainVec = np.array( self.gainOutputProtocol.data(), copy = False )

            # Set the object for both ears.
            # Note: Incorporate dynamically computed ILD if selected or adjust 
            # the level using an analytic model.
            gainVec[0:self.numberOfObjects] = self.levels
            gainVec[self.numberOfObjects:] = self.levels

            if self.hrirInterpolation:

                allGains =  self.inverted @ self.sourcePos.T
                minGains = np.min( allGains, axis = 1 ) # Minimum over last axis
                matchingTriplet = np.argmax( minGains, axis = 0 )

                #Select the gains for the matching triplets.
                unNormedGains = allGains[matchingTriplet,:,range(0,self.numberOfObjects)]
                gainNorm = np.linalg.norm( unNormedGains, ord=1, axis = -1 )
                normedGains = np.repeat( gainNorm[:,np.newaxis], 3, axis=-1 ) * unNormedGains
            else:
                 dotprod = self.hrirPos @ self.sourcePos.T
                 indices = np.argmax( dotprod, axis = 0 )

            if self.hrirInterpolation:
                _indices = self.hrirLookup.simplices[matchingTriplet,:]
                _interpFilters = np.einsum('ijkw,ij->ikw', self.hrirs[_indices,:,:], normedGains)

                for chIdx in range(0,self.numberOfObjects):
                    _leftInterpolant = pml.IndexedVectorFloat( chIdx, _interpFilters[chIdx,0,:] )
                    _rightInterpolant = pml.IndexedVectorFloat( chIdx+self.numberOfObjects, _interpFilters[chIdx,1,:] )
                    self.filterOutputProtocol.enqueue( _leftInterpolant )
                    self.filterOutputProtocol.enqueue( _rightInterpolant )
                 
                if self.dynamicITD:
                    delays = np.squeeze(np.matmul( np.moveaxis(self.dynamicDelays[_indices,:],1,2), _gnorm[...,np.newaxis]),axis=2 )
                    delayVec[0:self.numberOfObjects] = delays[:,0]
                    delayVec[self.numberOfObjects:] = delays[:,1]

            else: # hrirInterpolation == False
                for chIdx in range(0,self.numberOfObjects):    
                    if self.lastFilters[chIdx] != indices[chIdx]:
                        leftCmd  = pml.IndexedVectorFloat( chIdx,
                                                          self.hrirs[indices[chIdx],0,:])
                        rightCmd = pml.IndexedVectorFloat( chIdx+self.numberOfObjects,
                                                          self.hrirs[indices[chIdx],1,:])
                        self.filterOutputProtocol.enqueue( leftCmd )
                        self.filterOutputProtocol.enqueue( rightCmd )
                        self.lastFilters[chIdx] = indices[chIdx]

                        if self.dynamicITD:
                            delays = self.dynamicDelays[indices[chIdx],:]
                            delayVec[ [chIdx, chIdx + self.numberOfObjects] ] = delays
                        else:
                            delayVec[ [chIdx, chIdx + self.numberOfObjects] ] = 0.

            self.gainOutputProtocol.swapBuffers()
            self.delayOutputProtocol.swapBuffers()
            self.objectInputProtocol.resetChanged()
            if self.useHeadTracking:
                self.trackingInputProtocol.resetChanged()
