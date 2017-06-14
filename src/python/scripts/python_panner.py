# -*- coding: utf-8 -*-
"""
Created on Sun Feb 26 16:41:01 2017

@author: andi
"""

import visr
import panning
import pml
import objectmodel as om

import numpy as np
import matplotlib.pyplot as plt

class PythonPanner( visr.AtomicComponent ):
    def __init__( self, context, name, parent,
                 arrayConfig,
                 numberOfObjects=1,):
        super( PythonPanner, self ).__init__( context, name, parent )
        self.numLsp = arrayConfig.numberOfRegularLoudspeakers
        self.objectInput = visr.ParameterInput( "objectVectorInput", self,
                                               parameterType=pml.ObjectVector.staticType,
                                               protocolType=pml.DoubleBufferingProtocol.staticType,
                                               parameterConfig=pml.EmptyParameterConfig() )
        self.gainOutput = visr.ParameterOutput( "gainOutput", self,
                                               parameterType=pml.MatrixParameterFloat.staticType,
                                               protocolType=pml.SharedDataProtocol.staticType,
                                               parameterConfig=pml.MatrixParameterConfig(self.numLsp, numberOfObjects) )
        self.vbap = panning.VBAP( arrayConfig )
        
    def process( self ):
        ip = self.objectInput.protocolInput()
        gains = np.array(self.gainOutput.protocolOutput().data(), copy=False)
        if ip.changed(): # New data arrived?
            objectVec = ip.data()
            numObjects = objectVec.size
            for objIdx in range(0,numObjects):
                obj = objectVec.at( objIdx )
                if obj.type != om.ObjectType.PointSource:
                    continue
                outChannel = obj.channels[0]
                objGains = np.zeros( self.numLsp, dtype=np.float32 )
                self.vbap.calculateGains( obj.x, obj.y, obj.z, objGains )
                
                objGains = np.sqrt(np.clip(objGains, 0.0, np.inf ) );
                l2Norm = np.linalg.norm( objGains, ord=2 )
                objGains = 1.0/l2Norm * objGains
                gains[:,outChannel] = objGains
                
            ip.resetChanged()
