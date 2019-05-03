# -*- coding: utf-8 -*-

# Copyright (C) 2017-2018 Andreas Franck and Giacomo Costantini
# Copyright (C) 2017-2018 University of Southampton

# VISR Binaural Synthesis Toolkit (BST)
# Authors: Andreas Franck and Giacomo Costantini
# Project page: http://cvssp.org/data/s3a/public/BinauralSynthesisToolkit/


# The Binaural Synthesis Toolkit is provided under the ISC (Internet Systems Consortium) license
# https://www.isc.org/downloads/software-support-policy/isc-license/ :

# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
# OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


# We kindly ask to acknowledge the use of this software in publications or software.
# Paper citation:
# Andreas Franck, Giacomo Costantini, Chris Pike, and Filippo Maria Fazi,
# “An Open Realtime Binaural Synthesis Toolkit for Audio Research,” in Proc. Audio Eng.
# Soc. 144th Conv., Milano, Italy, 2018, Engineering Brief.
# http://www.aes.org/e-lib/browse.cfm?elib=19525

# The Binaural Synthesis Toolkit is based on the VISR framework. Information about the VISR,
# including download, setup and usage instructions, can be found on the VISR project page
# http://cvssp.org/data/s3a/public/VISR .

# VISR core packages
import visr
import pml
import objectmodel as om
import rbbl

# Helper functions contained in the
from visr_bst.util import cart2sph
from visr_bst.util import allSphHarmRealACN

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
        """
        Constructor.

        Parameters
        ----------
        numberOfObjects: int
            The maximum number of audio objects to be rendered.
        hoaOrder: int
            The Ambisonics order for encoding the objects.
        channelAllocation: bool, optional
            Whether to send dynamic channel allocation data. Not used at the moment.
            Default value means that the object channels are allocated statically and correspond to the
            obbject's channel id.
        """
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
                levels = np.zeros( self.numberOfObjects, dtype=coeffOut.dtype )
                pos = np.zeros( (self.numberOfObjects,3), dtype=coeffOut.dtype )
                pos[:,0] = 1.0 # Set unused sources to a valid direction.
                for src in ov:
                    chIdx = src.channels[0]
                    if chIdx > self.numberOfObjects:
                        warnings.warn('The number of dynamically instantiated sound objects is more than the maximum number specified')
                        continue
                    levels[chIdx] = src.level
                    pos[chIdx,:] = src.position
                sphPos = cart2sph( pos[:,0], pos[:,1], pos[:,2] )
                shCoeffs = allSphHarmRealACN( self.hoaOrder, np.pi/2-sphPos[1,:], sphPos[0,:], dtype = coeffOut.dtype )
                res = shCoeffs * levels[np.newaxis,:]
                coeffOut[...] = res
# Old, serialized code:
#                for index,src in enumerate(ov):
#                    if index < self.numberOfObjects :
#                        chIdx = src.channels[0]
#                        sph = cart2sph( src.x, src.y, src.z )
#                        pwCoeffs = allSphHarmRealACN( self.hoaOrder, np.pi/2-sph[1], sph[0], dtype = coeffOut.dtype )
#                        coeffOut[ :, chIdx ] = pwCoeffs * src.level # encode the object level in the coefficients
#                    else:
#                        warnings.warn('The number of dynamically instantiated sound objects is more than the maximum number specified')
#                        break