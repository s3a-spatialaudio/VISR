# -*- coding: utf-8 -*-
"""
Copyright Institute of Sound and Vibration research - All rights reserved

S3A Binaural toolbox for the VISR framework

Created on Wed Sep  6 22:02:40 2017

@author: Andreas Franck a.franck@soton.ac.uk 
"""

from readSofa import sph2cart

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
                  hrirInterpolation = False # HRTF interpolation selection: False: Nearest neighbour, True: Barycentric (3-point) interpolation
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
            self.trackingInput = visr.ParameterInput( "headPosition", self, pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )
            self.trackingInputProtocol = self.trackingInput.protocolInput()

        else:
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
        else:
            self.delayOutput = None
            
        if dynamicILD:
            self.gainOutput = visr.ParameterOutput( "gainOutput", self,
                                                pml.VectorParameterFloat.staticType,
                                                pml.DoubleBufferingProtocol.staticType,
                                                pml.VectorParameterConfig( 2*self.numberOfObjects) )
        else:
            self.gainOutput = None
            
        # HRIR selection and interpolation data
        self.hrirs = np.array( hrirData, copy = True, dtype = np.float32 )
        self.hrirPos = sph2cart(np.array( hrirPositions, copy = True, dtype = np.float32 ))
        self.hrirInterpolation = hrirInterpolation
        if self.hrirInterpolation:
            self.lastPosition = np.repeat( [[np.NaN, np.NaN, np.NaN]], self.numberOfObjects, axis=0 )
        else:
            self.lastFilters = np.repeat( -1, self.numberOfObjects, axis=0 )
        self.hrirLookup = KDTree( self.hrirPos )
        
        # %% Dynamic allocation of objects to channels
        self.channelAllocator = rbbl.ObjectChannelAllocator( self.numberOfObjects )
        
        
    def process( self ):
        if self.objectInputProtocol.changed():
            ov = self.objectInputProtocol.data();
            
            objIndicesRaw = [x.objectId for x in ov
                          if isinstance( x, (om.PointSource, om.PlaneWave) ) ]
            self.channelAllocator.setObjects( objIndicesRaw )
            objIndices = self.channelAllocator.getObjectChannels()
            for objIdx in objIndices:
                src = ov[objIdx]
                
            
            
            self.objectInputProtocol.resetChanged()
