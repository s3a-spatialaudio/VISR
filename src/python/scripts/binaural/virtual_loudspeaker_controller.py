#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Oct 26 16:46:32 2017

@author: gc1y17
"""

from numpy.linalg import inv
from readSofa import sph2cart
from rotationFunctions import calcRotationMatrix, cart2sph, sph2cart3inp, rad2deg
import visr
import pml
import time
import cProfile, pstats, io
import numpy as np

from scipy.spatial import ConvexHull

class VirtualLoudspeakerController( visr.AtomicComponent ):
    """Controller component for a dynamic (head-tracked) virtual loudspeaker (or binaural room scanning) renderer."""

    def __init__( self,
                  context, name, parent,    # Standard visr component constructor arguments
                  numberOfLoudspeakers,          # The number of point source objects rendered.
                  hrirPositions,            # The directions of the HRTF measurements, given as a #hrirPos x 2 or #hrirPos x 3 array,
                                            # in polar or spherical coordinates, respectively
                  hrirData,                 # The HRTF data as #hrirPos x 2 x #lsp x #firTaps matrix.
                  headRadius = 0.0875,      # Head radius, optional. Might be used in a dynamic ITD/ILD individualisation algorithm.
                  useHeadTracking = False,  # Whether head tracking data is provided via a self.headOrientation port.
                  dynamicITD = False,       # Whether ITD delays are calculated and sent via a "delays" port.
                  hrirInterpolation = False, # HRTF interpolation selection: False: Nearest neighbour, True: Barycentric (3-point) interpolation
                  delays = None,             # Matrix of delays associated with filter dataset. Dimension: #hrirPos x 2 x #lsp
                  interpolatingConvolver = False # Whether to transmit interpolation parameters (True) or complete interpolated filters
                  ):
        # Call base class (AtomicComponent) constructor
        super( VirtualLoudspeakerController, self ).__init__( context, name, parent )
        self.numberOfLoudspeakers = numberOfLoudspeakers
        self.dynamicITD = dynamicITD

        # %% Define parameter ports
        if useHeadTracking:
            self.useHeadTracking = True
            self.trackingInput = visr.ParameterInput( "headTracking", self, pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )
            self.trackingInputProtocol = self.trackingInput.protocolInput()

        else:
            self.useHeadTracking = False
            self.trackingInputProtocol = None # Flag that head tracking is not used.

        if interpolatingConvolver:
            self.filterOutputProtocol = None # Used as flag to distinguish between the output modes.

            if not hrirInterpolation:
                numInterpolants = 1
            elif hrirPositions.shape[-1] == 2:
                numInterpolants = 2
            else:
                numInterpolants = 3

            self.interpolationOutput = visr.ParameterOutput( "interpolatorOutput", self,
                                                     pml.InterpolationParameter.staticType,
                                                     pml.MessageQueueProtocol.staticType,
                                                     pml.InterpolationParameterConfig(numInterpolants) )
            self.interpolationOutputProtocol = self.interpolationOutput.protocolOutput()
        else:
            self.filterOutput = visr.ParameterOutput( "filterOutput", self,
                                                     pml.IndexedVectorFloat.staticType,
                                                     pml.MessageQueueProtocol.staticType,
                                                     pml.EmptyParameterConfig() )
            self.filterOutputProtocol = self.filterOutput.protocolOutput()
            self.interpolationOutputProtocol = None

        if self.dynamicITD:
            if (delays is None) or (delays.ndim != 3) or (delays.shape != hrirData.shape[0:-1] ):
                raise ValueError( 'If the "dynamicITD" option is given, the parameter "delays" must be a #hrirs x 2 matrix.' )

            self.dynamicDelays = np.array(delays, copy=True)
            self.delayOutput = visr.ParameterOutput( "delayOutput", self,
                                                    pml.VectorParameterFloat.staticType,
                                                    pml.DoubleBufferingProtocol.staticType,
                                                    pml.VectorParameterConfig( 2*self.numberOfLoudspeakers) )
            self.delayOutputProtocol = self.delayOutput.protocolOutput()
        else:
            self.dynamicDelays = None

        # Store HRIR selection and interpolation data unless only interpolation
        # parameters are transmitted.
        if interpolatingConvolver:
            self.hrirs = None
            self.numSpeakers = hrirData.shape[2]
        else:
            self.hrirs = np.array( hrirData, copy = True, dtype = np.float32 )

        hrirPosDim = hrirPositions.shape[-1] # Whether we are using a 2D or 3D grid
        if hrirPosDim == 2:
            # polar->Cartesian plus normalisation to unit radius
            self.hrirPos = np.array(np.stack( ( np.cos(hrirPositions[:,0]), np.sin(hrirPositions[:,0])), axis = 1), dtype=np.float32 )
            self.headDir = np.array([1.0, 0.0 ], dtype=self.hrirPos.dtype )
        elif hrirPosDim == 3:
            self.hrirPos = sph2cart( hrirPositions[:,0], hrirPositions[:,1], 1.0 )
            self.headDir = np.array([1.0, 0.0, 0.0 ], dtype=self.hrirPos.dtype )
        else:
            raise ValueError( 'HRIR position data has unsupported vector dimension.' )

        self.hrirInterpolation = hrirInterpolation
        if self.hrirInterpolation:
            self.hrirLookup = ConvexHull( self.hrirPos )
            self.triplets = np.array(np.transpose(self.hrirLookup.points[self.hrirLookup.simplices], axes=(0, 2, 1)),
                                                  dtype=self.hrirPos.dtype)
            self.inverted = inv(self.triplets)
#            print(self.triplets )
        else:
            self.lastHrirIndex = -1

        # Initialise the head direction.
        # TODO: This should be costumisable (especially if head tracking is deactivated)
        # TODO: make it 2D or 3D depending on the dimension of hrirPositions
        # For the time being we assume that it is 2D
        self.headDir = np.array([1.0, 0.0 ], dtype=self.hrirPos.dtype )

    def process( self ):
        # TODO: This belongs somewhere else in the recompute logic.
        if self.useHeadTracking and self.trackingInputProtocol.changed():
            htrack = self.trackingInputProtocol.data()
            ypr = htrack.orientation

            if self.hrirPos.shape[-1] == 2:
                # 2D hrir positions
                self.headDir[...] = sph2cart3inp(ypr[0],0,1)[:2]
            else: # 3D positions
                # TODO: Check and test this!
                rotationMatrix = calcRotationMatrix( - ypr)
                self.headDir[...] = np.array(np.matmul(np.asarray([1.0, 0.0, 0.0], dtype=self.hrirPos.dtype),rotationMatrix))

            self.trackingInputProtocol.resetChanged()

        # Obtain access to the delay output
        if self.dynamicITD:
            delayVec = np.array( self.delayOutputProtocol.data(), copy = False )

        if self.hrirInterpolation:
            allGains =  self.inverted @ self.headDir.T
            minGains = np.min( allGains, axis = 1 ) # Minimum over last axis
            matchingSimplex = np.argmax( minGains, axis = 0 )
            #Select the gains for the matching simplices.
            unNormedGains = allGains[matchingSimplex,:]
            gainNorm = np.linalg.norm( unNormedGains, ord=1, axis = -1 )
            normedGains = unNormedGains / gainNorm

            indices = self.hrirLookup.simplices[matchingSimplex,:]
            if self.interpolationOutputProtocol is None:
                interpFilters = np.einsum('jkiw,j->ikw', self.hrirs[indices,...], normedGains)

            for lspIdx in range(0,self.numberOfLoudspeakers):
                if self.interpolationOutputProtocol is not None:
                    # Part of computation could be moved outside.
                    leftIndices = (indices*2+0)*self.numberOfLoudspeakers + lspIdx
                    rightIndices = (indices*2+1)*self.numberOfLoudspeakers + lspIdx
                    leftInterpParameter = pml.InterpolationParameter( lspIdx,
                                                                     leftIndices.tolist(),
                                                                     normedGains.tolist() )
                    rightInterpParameter = pml.InterpolationParameter( lspIdx+self.numberOfLoudspeakers,
                                                                     rightIndices.tolist(),
                                                                     normedGains.tolist() )
                    self.interpolationOutputProtocol.enqueue( leftInterpParameter )
                    self.interpolationOutputProtocol.enqueue( rightInterpParameter )
                else:
                    leftInterpolant = pml.IndexedVectorFloat( lspIdx, interpFilters[lspIdx,0,:] )
                    rightInterpolant = pml.IndexedVectorFloat( lspIdx+self.numberOfLoudspeakers, interpFilters[lspIdx,1,:] )
                    self.filterOutputProtocol.enqueue( leftInterpolant )
                    self.filterOutputProtocol.enqueue( rightInterpolant )

            if self.dynamicITD:
                delays = np.dot( np.moveaxis(self.dynamicDelays[indices,:],0,-1), normedGains)
                delayVec[0:self.numberOfLoudspeakers] = delays[0,:]
                delayVec[self.numberOfLoudspeakers:] = delays[1,:]

        else:  # hrirInterpolation == False
            dotprod = self.hrirPos @ self.headDir.T
            hrirIndex = np.argmax( dotprod, axis = 0 )
            if self.lastHrirIndex != hrirIndex:
                for chIdx in range(0,self.numberOfLoudspeakers):
                    leftCmd  = pml.IndexedVectorFloat( chIdx,
                                                      self.hrirs[hrirIndex,0,chIdx,:])
                    rightCmd = pml.IndexedVectorFloat( chIdx+self.numberOfLoudspeakers,
                                                      self.hrirs[hrirIndex,1,chIdx,:])
                    self.filterOutputProtocol.enqueue( leftCmd )
                    self.filterOutputProtocol.enqueue( rightCmd )

                if self.dynamicITD:
                    delayVec[0:self.numberOfLoudspeakers] = self.dynamicDelays[hrirIndex,0,:]
                    delayVec[self.numberOfLoudspeakers:] = self.dynamicDelays[hrirIndex,1,:]

                self.lastHrirIndex = hrirIndex
        if self.dynamicITD:
            self.delayOutputProtocol.swapBuffers()