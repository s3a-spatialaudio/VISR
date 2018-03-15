# -*- coding: utf-8 -*-

# %BST_LICENCE_TEXT%


# VISR core packages
import visr
import pml
import objectmodel as om
import rbbl

# Helper functions contained in the
from .util.rotation_functions  import cart2sph
from .util.real_spherical_harmonics import allSphHarmRealACN

# Standard Python packages
import numpy as np
import warnings

class HoaObjectEncoder( visr.AtomicComponent ):
    """
    Component to calculate encoding coefficients for point source and plane wave audio objects contained in
    an object vector.
    """
    def __init__( self,
                  context, name, parent,     # Standard visr component constructor arguments
                  numberOfObjects,           # The number of point source objects rendered.
                  hoaOrder,                  # The Ambisonics order for encoding the objects
                  channelAllocation = False  # Whether to allocate object channels dynamically (not used at the moment)
                  ):
        # Call base class (AtomicComponent) constructor
        super( HoaObjectEncoder, self ).__init__( context, name, parent )
        self.numberOfObjects = numberOfObjects
        self.hoaOrder = hoaOrder
        self.numHoaCoeffs = (self.hoaOrder+1)**2

        # %% Define parameter ports
        self.objectInput = visr.ParameterInput( "objectVector", self, pml.ObjectVector.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )
        self.objectInputProtocol = self.objectInput.protocolInput()

        matrixConfig = pml.MatrixParameterConfig( self.numHoaCoeffs, self.numberOfObjects )
        self.coefficientOutput = visr.ParameterOutput( "coefficientOutput", self,
                                                pml.MatrixParameterFloat.staticType,
                                                pml.SharedDataProtocol.staticType,
                                                matrixConfig )
        self.coefficientOutputProtocol = self.coefficientOutput.protocolOutput()

        if channelAllocation:
            self.channelAllocator = rbbl.ObjectChannelAllocator( self.numberOfObjects )
            self.usedChannels = set() # Initialised with an empty set.
            self.routingOutput = visr.ParameterOutput( "routingOutput", self,
                                                     pml.SignalRoutingParameter.staticType,
                                                     pml.DoubleBufferingProtocol.staticType,
                                                     pml.EmptyParameterConfig() )
            self.routingOutputProtocol = self.routingOutput.protocolOutput()
        else:
            self.routingOutputProtocol = None
            self.channelAllocator = None

    def process( self ):
        if self.objectInputProtocol.changed():
            ov = self.objectInputProtocol.data();

            coeffOut = np.array( self.coefficientOutputProtocol.data(), copy=False )
            if coeffOut.shape != ( self.numHoaCoeffs, self.numberOfObjects ):
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
                    if isinstance( src, om.PlaneWave ):
                        sph = [src.azimuth, src.elevation, src.referenceDistance ]
                    else:
                        sph = cart2sph( src.x, src.y, src.z )
                    pwCoeffs = allSphHarmRealACN( self.hoaOrder, np.pi/2-sph[1], sph[0], dtype = coeffOut.dtype )
                    coeffOut[ :, chIdx] = pwCoeffs * src.level # encode the object level in the coefficients
            else:
                for index,src in enumerate(ov):
                    if index < self.numberOfObjects :
                        chIdx = src.channels[0]
                        sph = cart2sph( src.x, src.y, src.z )
                        pwCoeffs = allSphHarmRealACN( self.hoaOrder, np.pi/2-sph[1], sph[0], dtype = coeffOut.dtype )
                        coeffOut[ :, chIdx ] = pwCoeffs * src.level # encode the object level in the coefficients
                    else:
                        warnings.warn('The number of dynamically instantiated sound objects is more than the maximum number specified')
                        break