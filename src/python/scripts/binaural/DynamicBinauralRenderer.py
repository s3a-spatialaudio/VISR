#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Sep 13 12:56:27 2017

@author: gc1y17
"""

import visr
import pml
import objectmodel as om
import rrl
import h5py
import rcl

#import objectmodel as om

import numpy as np;
import os
from readSofa import readSofaFile
from dynamic_binaural_controller import DynamicBinauralController
from urllib.request import urlretrieve

class DynamicBinauralRenderer( visr.CompositeComponent ):
    
        def __init__( self,
                     context, name, parent, 
                     numberOfObjects,
                     interpolationPeriod, 
                     diffusionFilters, 
                     trackingConfiguration='', 
                     sceneReceiverPort=4242,
                     frequencyDependentPanning=False ):
            super( DynamicBinauralRenderer, self ).__init__( context, name, parent )
            self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
            self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
            self.objectVectorInput = visr.ParameterInput( "objectDataInput", self, pml.ObjectVector.staticType,
                                                         pml.DoubleBufferingProtocol.staticType,
                                                         pml.EmptyParameterConfig() )
    
   
            sofaFile = './data/dtf b_nh169.sofa'

            if not os.path.exists( sofaFile ):
                urlretrieve( 'http://sofacoustics.org/data/database/ari%20(artificial)/dtf%20b_nh169.sofa',
                       sofaFile )

# hrirFile = 'c:/local/s3a_af/subprojects/binaural/dtf b_nh169.sofa'

            [ hrirPos, hrirData ] = readSofaFile( sofaFile )
            self.dynamicBinauraController = DynamicBinauralController( context, "DynamicBinauralController", None,
                                                                      numberOfObjects,
                                                                      hrirPos, hrirData,
                                                                      useHeadTracking = False,
                                                                      dynamicITD = True,
                                                                      dynamicILD = True,
                                                                      hrirInterpolation = False
                                                                      )
            
            self.parameterConnection( self.objectVectorInput, self.dynamicBinauraController.parameterPort("objectDataInput"))

            ## Load the BBC BRIR dataset
            brirFile = os.path.join( os.getcwd(), 'BBC_BRIR.mat' )
            brirMat =  h5py.File( brirFile )
            brirFull = np.array( brirMat['h_sweetspot'], dtype=np.float32 ).copy('C')
            # Scalefactor to compensate for the very low amplitudes of the BBC BRIRs
            brirScaleFactor = 500;
            brirFlat = brirScaleFactor * np.concatenate( (brirFull[:,0,:], brirFull[:,1,:] ) ) 
            brirFilters = pml.MatrixParameterFloat( brirFlat, 16 )
            numBrirSpeakers = brirFull.shape[0]
            # Define the routing for the binaural convolver such that it matches the organisation of the
            # flat BRIR matrix.
            filterRouting = pml.FilterRoutingList()
            for idx in range(0, numBrirSpeakers ):
                filterRouting.addRouting( idx, 0, idx, 1.0 )
                filterRouting.addRouting( idx, 1, idx+numBrirSpeakers, 1.0 )
                
            numFilters = brirFilters.numberOfRows
            firLength = brirFilters.numberOfColumns
            numRoutings = filterRouting.size
            self.convolver = rcl.FirFilterMatrix( context, 'covolutionEngine', self )
            self.convolver.setup( numberOfInputs=numberOfObjects,
                             numberOfOutputs=2,
                             maxFilters=numFilters,
                             filterLength=firLength,
                             maxRoutings=numRoutings,
                             filters=brirFilters,
                             routings=filterRouting,
                             controlInputs=False
                             )
            self.audioConnection(self.objectSignalInput, self.convolver.audioPort("in") )
            

            blockSize = 16
            self.delayVector = rcl.DelayVector( context, "delayVector" )
            self.delayVector.setup(2, interpolationType="lagrangeOrder3", initialDelay=0,
             controlInputs=True, initialGain=1.0, interpolationSteps=3*blockSize)

            self.adder = rcl.Add( context, 'add', numInputs = numberOfObjects, width=2)
            
            self.audioConnection( self.adder.audioPort("out"), self.binauralOutput)
            
            