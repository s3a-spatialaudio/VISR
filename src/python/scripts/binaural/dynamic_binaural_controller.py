# -*- coding: utf-8 -*-
"""
Copyright Institute of Sound and Vibration research - All rights reserved

S3A Binaural toolbox for the VISR framework

Created on Wed Sep  6 22:02:40 2017

@author: Andreas Franck a.franck@soton.ac.uk 
"""
from numpy.linalg import inv 
from readSofa import sph2cart
from rotationFunctions import calcRotationMatrix, cart2sph, rad2deg
import matplotlib.pyplot as plt
import visr
import pml
import rbbl
import objectmodel as om
import time

import numpy as np
from scipy.spatial import Delaunay
from scipy.spatial import KDTree
from scipy.spatial import ConvexHull
import plotly.plotly as py
import plotly.figure_factory as FF
from plotly.graph_objs import graph_objs

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
                  hrirInterpolation = False, # HRTF interpolation selection: False: Nearest neighbour, True: Barycentric (3-point) interpolation
                  channelAllocation = False  # Whether to allocate object channels dynamically
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
            self.useHeadTracking = True
            self.trackingInput = visr.ParameterInput( "headTracking", self, pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )
            self.trackingInputProtocol = self.trackingInput.protocolInput()

        else:
            self.useHeadTracking = False
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
            self.delayOutputProtocol = self.delayOutput.protocolOutput()
        else:
            self.delayOutputProtocol = None

        # We are always using the gain oputput matrix to set set the level, even if we do not use a loudness model.
        self.gainOutput = visr.ParameterOutput( "gainOutput", self,
                                               pml.VectorParameterFloat.staticType,
                                               pml.DoubleBufferingProtocol.staticType,
                                               pml.VectorParameterConfig( 2*self.numberOfObjects) )
        self.gainOutputProtocol = self.gainOutput.protocolOutput()

        if channelAllocation:
            self.routingOutput = visr.ParameterOutput( "routingOutput", self,
                                                     pml.SignalRoutingParameter.staticType,
                                                     pml.DoubleBufferingProtocol.staticType,
                                                     pml.EmptyParameterConfig() )
            self.routingOutputProtocol = self.routingOutput.protocolOutput()
        else:
            self.routingOutputProtocol = None

        # HRIR selection and interpolation data
        self.hrirs = np.array( hrirData, copy = True, dtype = np.float32 )
        
        # Normalise the hrir positions to unit radius (to let the k-d tree 
        # lookup work as expected.)
        hrirPositions[:,2] = 1.0
        self.hrirPos = sph2cart(np.array( hrirPositions, copy = True, dtype = np.float32 ))
        self.hrirInterpolation = hrirInterpolation
        if self.hrirInterpolation:
            self.lastPosition = np.repeat( [[np.NaN, np.NaN, np.NaN]], self.numberOfObjects, axis=0 )
            self.hrirLookup = ConvexHull( self.hrirPos )
            self.triplets = np.transpose(self.hrirLookup.points[self.hrirLookup.simplices], axes=(0, 2, 1))
            self.inverted = inv(self.triplets)
#            print(self.hrirPos)
        else:
            self.lastFilters = np.repeat( -1, self.numberOfObjects, axis=0 )
            self.hrirLookup = KDTree( self.hrirPos )

## PRINT 3d convex hull result
#        fig1 = FF.create_trisurf(x=self.hrirPos[:,0],y=self.hrirPos[:,1],z=self.hrirPos[:,2],
#                         simplices=self.hrirLookup.simplices,
#                         title="TRI", aspectratio=dict(x=1, y=1, z=1))
#        plotly.offline.plot(fig1, filename="TRI")


## PLOT the hrir positions on xy plane
#        plt.figure(1)            
#        for vec in self.hrirPos:
#            
#            plt.plot(vec[0], vec[1], '+')
#
#        plt.show() # or savefig(<filename>)

## PLOT the hrir positions on xz plane
#        plt.figure(2)            
#        for vec in self.hrirPos:
#            plt.plot(vec[0], vec[2], '+')
#        plt.show() # or savefig(<filename>)
        
## WRITE polar coordinates of HRIR positions into a file
#        f = open('workfile.txt', 'w')
#        for index, g in enumerate(self.hrirPos):
#           sph = cart2sph(g[0],g[1],g[2])
#           f.write('%d [%d %d]\n' % (index,rad2deg(sph[0]),rad2deg(sph[1])))
            
        # %% Dynamic allocation of objects to channels
        if channelAllocation:
            self.channelAllocator = rbbl.ObjectChannelAllocator( self.numberOfObjects )
            self.usedChannels = set()
        else:
            self.channelAllocator = None
            self.sourcePos = np.repeat( np.array([[1.0,0.0,0.0]]), self.numberOfObjects, axis = 0 )
            self.levels = np.zeros( (self.numberOfObjects), dtype = np.float32 )
        self.f = open('srcpAllinone.txt', 'w')

        
    def process( self ):
        startTot = time.time()
        if self.objectInputProtocol.changed():
            ov = self.objectInputProtocol.data();
                                              
            objIndicesRaw = [x.objectId for x in ov
                          if isinstance( x, (om.PointSource, om.PlaneWave) ) ]
            if self.channelAllocator is not None:
                self.channelAllocator.setObjects( objIndicesRaw )
                objIndices = self.channelAllocator.getObjectChannels()
                numObjects = len(objIndices)
                self.sourcePos = np.zeros( (numObjects,3), dtype=np.float32 )
                self.levels = np.zeros( (numObjects), dtype=np.float32 )
                
                for chIdx in range(0, numObjects):
                    objIdx = objIndices[chIdx]
                    self.sourcePos[chIdx,:] = ov[objIdx].position
                    self.levels[chIdx] = ov[objIdx].level
            else:
                self.levels[:] = 0.0
                for src in ov:
                    pos = np.asarray(src.position, dtype=np.float32 )
                    posNormed = 1.0/np.sqrt(np.sum(np.square(pos))) * pos
                    ch = src.channels[0]
                    self.sourcePos[ch,:] = posNormed
                    self.levels[ch] = src.level
                               
            if self.useHeadTracking:
                 if self.trackingInputProtocol.changed():
                     htrack = self.trackingInputProtocol.data()
                     ypr = htrack.orientation
                     
                     # np.negative is to obtain the opposite rotation of the head rotation, i.e. the inverse matrix of head rotation matrix
                     rotationMatrix = calcRotationMatrix(np.negative(ypr))
#                     rotationMatrix = np.identity(3)
#                     print(self.sourcePos.shape)

                     self.sourcePos = np.array(np.matmul(self.sourcePos,rotationMatrix.T))
                     
#PRINT OUT AZIMUTH AND ELEVATION OF FIRST SOURCE AFTER ROTATION                     
#                     sph1 = cart2sph(self.sourcePos[0][0],self.sourcePos[0][1],self.sourcePos[0][2])
#                     print("[%d %d]"%(rad2deg(sph1[0]),rad2deg(sph1[1])))   
                     


#ALTERNATIVE METHODS TO MULTIPLY SOURCEPOS*ROTATIONMATRIX

#                     self.sourcePos = self.sourcePos.dot(rotationMatrix[:,:3].T)[:,:3]
#                     print(self.sourcePos.shape)                     
                     
#                     for index,column in enumerate(np.matrix(self.sourcePos)):
#                         self.sourcePos[index,:] = (rotationMatrix*column.T).T                      


#SOURCEPOS DEBUG AFTER ROTATION
#                     print(self.sourcePos)
#                     print(self.sourcePos.shape)
#                     for srcp in self.sourcePos :
#                         self.f.write('[%f %f %f]\n' % (srcp.item(0),srcp.item(1),srcp.item(2)))
                    
            if self.hrirInterpolation:
              
                indices = np.zeros((self.numberOfObjects,3), dtype = np.int ) 
                for chIdx in range(0,self.numberOfObjects):
                     start = time.time()
#                     gtot = np.zeros((self.triplets.shape[0],3), dtype = np.float32 ) 
                     gtot = np.matmul(self.inverted,self.sourcePos[chIdx,:])
#                     for trip in range(0,self.triplets.shape[0]):
#                         gtot[trip,:] = np.matmul(self.inverted[trip,:,:],self.sourcePos[chIdx,:])
#                             
#                     for gtt in gtot :
#                         self.f.write('[%f %f %f]\n' % (gtt.item(0),gtt.item(1),gtt.item(2)))
               
                     gMaxIndex = np.argmax(gtot.min(axis=1))
                     indices[chIdx] = self.hrirLookup.simplices[gMaxIndex]
                     gain = self.levels[chIdx]
#                     print("Triplet choice %f sec "%(time.time()-start))
            else:
                 [ d,indices ] = self.hrirLookup.query( self.sourcePos, 1, p =2 )
                                                    
            # Retrieve the output gain vector for setting the object level and potentially
            # applying dynamically computed 

            gainVec = self.gainOutputProtocol.data()
            for chIdx in range(0,self.numberOfObjects):
#                print("object n: "+str(chIdx))
                gain = self.levels[chIdx]
                # Set the object for both ears.
                # Note: Incorporate dynamically computed ILD is selected.
                gainVec[chIdx] = gain
                gainVec[chIdx+self.numberOfObjects] = gain
            
                # If the source is silent (probably inactive), don't change filters
                if gain >= 1.0e-7:
                    if self.hrirInterpolation:
#                        print(indices[chIdx])
                        start2 = time.time()
                        if not np.array_equal(self.lastPosition[chIdx],indices[chIdx]):
#                            print("changed")
                            threeNeighMatrix = self.hrirPos[indices[chIdx]].T                          
                            g = inv(threeNeighMatrix)*np.matrix(self.sourcePos[chIdx]).T
                            gnorm = g*1/np.linalg.norm(g,ord=1)

                            leftAccum =  np.zeros((1,self.hrirs[indices[0][0]][0].shape[0]),dtype = np.float32)
                            rightAccum = np.zeros((1,self.hrirs[indices[0][0]][1].shape[0]),dtype = np.float32)
           
                            for neighIdx in range(0,3):
                                leftCmd  = self.hrirs[indices[chIdx][neighIdx],0,:]
                                rightCmd = self.hrirs[indices[chIdx][neighIdx],1,:]                                

                                leftWeighted = gnorm[neighIdx] * np.array(leftCmd)
                                rightWeighted = gnorm[neighIdx]* np.array(rightCmd)
                                leftAccum += leftWeighted
                                rightAccum += rightWeighted

                            leftInterpolator = pml.IndexedVectorFloat( chIdx, leftAccum[0].tolist())
                            rightInterpolator = pml.IndexedVectorFloat( chIdx+self.numberOfObjects, rightAccum[0].tolist())
                            self.filterOutputProtocol.enqueue( leftInterpolator )
                            self.filterOutputProtocol.enqueue( rightInterpolator )
                            self.lastPosition[chIdx] = indices[chIdx]
#                        print("filter out %f sec"%(time.time()-start2))
                    else:
                        sph1 = cart2sph(self.hrirPos[indices[chIdx]][0],self.hrirPos[indices[chIdx]][1],self.hrirPos[indices[chIdx]][2])
                        print("%d:[%d %d]"%(indices[chIdx],rad2deg(sph1[0]),rad2deg(sph1[1])))
                        if self.lastFilters[chIdx] != indices[chIdx]:
                           
                            leftCmd  = pml.IndexedVectorFloat( chIdx,
                                                              self.hrirs[indices[chIdx],0,:])
                            rightCmd = pml.IndexedVectorFloat( chIdx+self.numberOfObjects,
                                                              self.hrirs[indices[chIdx],1,:])
                            self.filterOutputProtocol.enqueue( leftCmd )
                            self.filterOutputProtocol.enqueue( rightCmd )
                            self.lastFilters[chIdx] = indices[chIdx]
            
            self.gainOutputProtocol.swapBuffers()
            self.objectInputProtocol.resetChanged()
            if self.useHeadTracking:
                self.trackingInputProtocol.resetChanged()
#        print("TOT controller time %f sec"%(time.time()-startTot))