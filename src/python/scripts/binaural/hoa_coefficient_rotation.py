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

from rotationFunctions import cart2sph, calcRotationMatrix
from real_sph_harmonics import allSphHarmRealACN

import numpy as np

class HoaCoefficientRotation( visr.AtomicComponent ):
    """ Component encode point sources and plane waves contained in an object vector into a spherical harmonic coefficients """
    def __init__( self,
                  context, name, parent,    # Standard visr component constructor arguments
                  numberOfObjects,          # The number of point source objects supported.
                  hoaOrder,
                  ):
        # Call base class (AtomicComponent) constructor
        super( HoaCoefficientRotation, self ).__init__( context, name, parent )
        self.numberOfObjects = numberOfObjects
        self.hoaOrder = hoaOrder
        self.numHoaCoeffs = (self.hoaOrder+1)**2
        self.rotationMatrix = np.identity( 3, dtype = np.float32 ) # TODO: decide whether to use xyz or ACN (yzx) order

        # %% Define parameter ports
        matrixConfig = pml.MatrixParameterConfig( self.numHoaCoeffs, self.numberOfObjects )
        self.coefficientInput = visr.ParameterInput( "coefficientInput", self,
                                                pml.MatrixParameterFloat.staticType,
                                                pml.SharedDataProtocol.staticType,
                                                matrixConfig )
        self.coefficientInputProtocol = self.coefficientInput.protocolInput()
        self.coefficientOutput = visr.ParameterOutput( "coefficientOutput", self,
                                                pml.MatrixParameterFloat.staticType,
                                                pml.SharedDataProtocol.staticType,
                                                matrixConfig )
        self.coefficientOutputProtocol = self.coefficientOutput.protocolOutput()
        
        # Instantiate the head tracker input.
        self.trackingInput = visr.ParameterInput( "headTracking", self, pml.ListenerPosition.staticType,
                                                  pml.DoubleBufferingProtocol.staticType,
                                                  pml.EmptyParameterConfig() )
        self.trackingInputProtocol = self.trackingInput.protocolInput()

    def process( self ):

        if self.trackingInputProtocol.changed():
            head = self.trackingInputProtocol.data()
            ypr = - np.array(head.orientation, dtype = np.float32 ) # negative because we rotate the sound field in the opposite 
            # direction of the head orientation.
            self.rotationMatrix[...] = calcRotationMatrix( ypr )
            self.trackingInputProtocol.resetChanged()

        coeffIn = np.array( self.coefficientInputProtocol.data(), copy = False )
        coeffOut = np.array( self.coefficientOutputProtocol.data(), copy = False )

        # Order 0 remains unchanged
        coeffOut[0,:] = coeffIn[0,:]

        # TODO: If not done before, translate the rotation matrix to ACN format

        # Compute order order by order
        for order in range(1, self.hoaOrder+1):
            # TODO: Compute the transformation matrix for order 'order'
            rot = np.identity( 2*order + 1, dtype = coeffOut.dtype ) # Dummy rotation matrix

            coeffOut[ (order**2):((order+1)**2), : ] = np.matmul( rot, coeffIn[ (order**2):((order+1)**2), : ] )
