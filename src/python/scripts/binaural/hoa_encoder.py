# -*- coding: utf-8 -*-
"""
Copyright Institute of Sound and Vibration research - All rights reserved

S3A Binaural toolbox for the VISR framework

Created on Fri 06 Oct 2017

@author: Andreas Franck a.franck@soton.ac.uk 
"""
import visr
import pml
import objectmodel as om
# import time

from rotationFunctions import cart2sph
from real_sph_harmonics import allSphHarmRealACN

import numpy as np

class HoaEncoder( visr.AtomicComponent ):
    """ Component encode point sources and plane waves contained in an object vector into a spherical harmonic coefficients """
    def __init__( self,
                  context, name, parent,    # Standard visr component constructor arguments
                  numberOfObjects,          # The number of point source objects rendered.
                  hoaOrder,
                  channelAllocation = False  # Whether to allocate object channels dynamically
                  ):
        # Call base class (AtomicComponent) constructor
        super( HoaEncoder, self ).__init__( context, name, parent )
        self.numberOfObjects = numberOfObjects
        self.hoaOrder = hoaOrder
        self.numHoaCoeffs = (self.hoaOrder+1)**2

        # %% Define parameter ports
        self.objectInput = visr.ParameterInput( "objectVector", self, pml.ObjectVector.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )
        self.objectInputProtocol = self.objectInput.protocolInput()

        matrixConfig = pml.MatrixParameterConfig(self.numberOfObjects, self.numHoaCoeffs)
        self.coefficientOutput = visr.ParameterOutput( "coefficientOutput", self,
                                                pml.MatrixParameterFloat.staticType,
                                                pml.DoubleBufferingProtocol.staticType,
                                                matrixConfig )
        self.coefficientOutputProtocol = self.coefficientOutput.protocolOutput()

        if channelAllocation:
            self.routingOutput = visr.ParameterOutput( "routingOutput", self,
                                                     pml.SignalRoutingParameter.staticType,
                                                     pml.DoubleBufferingProtocol.staticType,
                                                     pml.EmptyParameterConfig() )
            self.routingOutputProtocol = self.routingOutput.protocolOutput()
        else:
            self.routingOutputProtocol = None

    def process( self ):
        if self.objectInputProtocol.changed():
            ov = self.objectInputProtocol.data();
            
            coeffOut = np.array( self.coefficientOutputProtocol.data(), copy=False )
            if coeffOut.shape != (self.numberOfObjects, self.numHoaCoeffs ):
                raise ValueError( 'The dimensions of the output coefficient matrix does not match the expected shape.' )
            coeffOut[...] = 0.0

            objIndicesRaw = [x.objectId for x in ov
                          if isinstance( x, (om.PointSource, om.PlaneWave) ) ]
            if self.channelAllocator is not None:
                self.channelAllocator.setObjects( objIndicesRaw )
                objIndices = self.channelAllocator.getObjectChannels()
                numObjects = len(objIndices)

                for chIdx in range(0, numObjects):
                    objIdx = objIndices[chIdx]
                    src = ov[objIdx]
                    sph = cart2sph( src.x, src.y, src.z )
                    pwCoeffs = allSphHarmRealACN( self.hoaOrder, np.pi/2-sph[1], sph[0], dtype = coeffOut.dtype )
                    coeffOut[chIdx, : ] = pwCoeffs * src.level # encode the object level in the coefficients
            else:
                self.levels[:] = 0.0
                for src in ov:
                    ch = src.channels[0]
                    sph = cart2sph( src.x, src.y, src.z )
                    pwCoeffs = allSphHarmRealACN( self.hoaOrder, np.pi/2-sph[1], sph[0], dtype = coeffOut.dtype )
                    coeffOut[ch, : ] = pwCoeffs * src.level # encode the object level in the coefficients
            
            self.coefficientOutputProtocol.swapBuffers()