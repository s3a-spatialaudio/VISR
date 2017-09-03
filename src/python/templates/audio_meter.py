# -*- coding: utf-8 -*-
"""
Created on Fri Sep  1 15:22:13 2017

@author: af5u13
"""

# Template for an atomic component that takes in an audio signal and outputs a
# stream of indicator values (meter outputs)

# %% Module imports

import visr   # Core VISR module, defines components and ports
import pml    # Parameter message library, defines standard parameter types and communication protocols.

import numpy as np
import scipy.signal as sig

# Definition of a component
class SurroundLoudnessMeter( visr. AtomicComponent ):
    # Defines the constructor that creates an SurroundLoudnessMeter. 
    def __init__( self, context, name, parent, 
                  numberOfChannels,
                  gatingPeriod ):
        # Call the base class constructor
        super(SurroundLoudnessMeter,self).__init__(context,name,parent)
        # Define an audio input port with name "audioIn" and width (number of signal waveforms) numberOfChannels
        self.audioInput = visr.AudioInputFloat( "audioIn", self, numberOfChannels )
        
        self.audioOutput = visr.AudioOutputFloat( "audioOut", self, numberOfChannels )
        
        # Define a parameter output port with type "Float" and communication protocol "MessageQueue"
        self.loudnessOut = visr.ParameterOutput( "loudnessOut", self, pml.Float.staticType, pml.MessageQueueProtocol.staticType, pml.EmptyParameterConfig() )
        
        # Setup data used in the process() function.
        
        # IIR filter state to be saved in betweem
        self.filterState = np.zeros( (2,numberOfChannels,2), dtype = np.float32 )
        
        # IIR coefficients for K-weighting, taken from ITU-R BS.1770-4
        # https://www.itu.int/dms_pubrec/itu-r/rec/bs/R-REC-BS.1770-4-201510-I!!PDF-E.pdf
        self.Kweighting = np.asarray( [[1.53512485958697, -2.69169618940638, 1.19839281085285, 1.0, -1.69065929318241, 0.73248077421585],
                                       [ 1.0, -2.0, 1.0, 1.0, -1.99004745483398, 0.99007225036621 ]], dtype = np.float32 )
    
        # Loudspeaker weightings. This assume a standard channel layout and fails for more than 5 channels.
        # TODO: Provide an option for the weightings.
        allWeightings = np.power( 10, np.array([0.0, 0.0, 0.0, 1.5, 1.5], dtype=np.float32)/20.0 )
        self.channelWeights = allWeightings[0:numberOfChannels]

    # The process() method implements the runtime operation of the component.
    # It is called regularly (every context.period samples) by the runtime system.
    def process( self ):
        # Retrieve the new input samples 
        x = self.audioInput.data()
        
        xk, self.filterState = sig.sosfilt( self.Kweighting, x, axis = 1, zi = self.filterState )
        
        y1 = np.mean( np.power( xk, 2 ), axis=1 )
                
        self.audioOutput.set( xk )
        
        # Compute the ungated loudness
        Lk = -0.691 + 10*np.log10( np.dot( y1, self.channelWeights ) )

        # Create a parameter message holding a single float.
        outParam = pml.Float( Lk )
        
        self.loudnessOut.protocolOutput().enqueue( outParam )