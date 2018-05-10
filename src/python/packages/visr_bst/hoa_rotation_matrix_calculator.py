# -*- coding: utf-8 -*-

# %BST_LICENCE_TEXT%

import numpy as np

import visr
import pml

from .util.rotation_functions import calcRotationMatrix, rotationMatrixReorderingACN
# Use the vectorised, faster function.
from .util.spherical_harmonics_rotation import allSphericalHarmonicsRotationMatrices

class HoaRotationMatrixCalculator( visr.AtomicComponent ):
    """
    Component compute a spherical harmonics rotation matrix. The matrix coeffients
    are output as a coefficient list for a sparse matrix.
    """
    def __init__( self,
                  context, name, parent,
                  hoaOrder,
                  dynamicOrientation = False,
                  initialOrientation = None
                  ):
        """
        Constructor.

        Parameters
        ----------
        context: visr.SignalFlowContext
            Structure containing block size and sampling frequency, standard visr component construction parameter.
        name: string
            Name of the component, can be chosen freely as long as it is unique withion the containing component.
        parent: visr.CompositeComponent or NoneType
            The containing composite component, or None for a top-level component.
        hoaOrder: int
            The spherical harmonics order, determines the size of the output matrix.
        dynamicOrientation: bool
            Whether the orientation is updated at runtime. If True, a parmater input
            "orientation" is instantiated that receivers pml.ListenerPositions
        initialOrientation: array-like (2- or 3- element) or NoneType
            The initial head rotation or the static head orientation if dynamic updates are deactivated. Given as yaw, pitch, roll.
        """
        # Call base class (AtomicComponent) constructor
        super( HoaRotationMatrixCalculator, self ).__init__( context, name, parent )
        self.hoaOrder = hoaOrder
        self.numHoaCoeffs = (self.hoaOrder+1)**2

        # Number of nonzero matrix coefficients.
        # Explisit formula sum( (2i+1)^2 ) for i = 0 .. hoaOrder
        # This is always an integer (i.e., multiple of 3 before division).
        self.numMatrixCoeffs = ((hoaOrder+1) * (2*hoaOrder +1 ) * (2*hoaOrder+3)) // 3

        # %% Define parameter ports
        outConfig = pml.VectorParameterConfig( self.numMatrixCoeffs )
        self.coeffOutput = visr.ParameterOutput( "coefficients", self,
                                                pml.VectorParameterFloat.staticType,
                                                pml.DoubleBufferingProtocol.staticType,
                                                outConfig )
        self.coeffOutputProtocol = self.coeffOutput.protocolOutput()

        if dynamicOrientation:
            self.orientationInput = visr.ParameterInput( "orientation", self, pml.ListenerPosition.staticType,
                                                      pml.DoubleBufferingProtocol.staticType,
                                                      pml.EmptyParameterConfig() )
            self.orientationInputProtocol = self.orientationInput.protocolInput()
        else:
            self.orientationInputProtocol is None

        if initialOrientation is None:
            initialOrientation = np.zeros( (3), np.float32 )
        else:
            initialOrientation = np.asarray( initialOrientation, dtype = np.float32 )
            if initialOrientation.size < 3:
                initialOrientation = np.concatenate( (initialOrientation,
                    np.zeros( 3-initialOrientation.size, dtype=np.float32) ) )
        R1 = rotationMatrixReorderingACN( calcRotationMatrix( initialOrientation ) )
        self.rotationMatrices = allSphericalHarmonicsRotationMatrices( self.hoaOrder, R1 )

    def process( self ):
        """
        Processing function, called from the runtime system in every iteration of the signal flow.
        """
        if (self.orientationInputProtocol is not None ) and self.orientationInputProtocol.changed():
            head = self.orientationInputProtocol.data()
            ypr = - np.array(head.orientation, dtype = np.float32 ) # negative because we rotate the sound field in the opposite
            # direction of the head orientation.
            R1 = rotationMatrixReorderingACN( calcRotationMatrix(ypr) )
            self.rotationMatrices = allSphericalHarmonicsRotationMatrices( self.hoaOrder, R1 )
            self.orientationInputProtocol.resetChanged()

        coeffOut = np.array( self.coeffOutputProtocol.data(), copy = False )

        for order,R in enumerate(self.rotationMatrices):
            startIdx = (order * (2*order+1) * (2*order-1)) // 3
            endIdx = ((order+1) * (2*order+1) * (2*order+3)) // 3

            # Write the sub-matrix row-by row.
            coeffOut[ startIdx:endIdx  ] = np.reshape( R, ((2*order+1)**2),  order='C' )

        self.coeffOutputProtocol.swapBuffers()
