#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Oct 26 16:46:32 2017

@author: gc1y17
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
import cProfile, pstats, io
import numpy as np
import warnings


import plotly.plotly as py
from matplotlib.patches import Polygon
import plotly.figure_factory as FF
from plotly.graph_objs import graph_objs

from scipy.spatial import Delaunay
from scipy.spatial import KDTree
from scipy.spatial import ConvexHull
def sph2cart3inp(az,el,r):
    x = r*np.cos(az)*np.cos(el)
    y = r*np.sin(az)*np.cos(el)
    z = r*np.sin(el)
    return np.stack((x,y,z),0)

class VirtualLoudspeakerController( visr.AtomicComponent ):
    """ Component to translate an object vector (and optionally head tracking information)
        into parameter parameters for dynamic binaural signal processing. """
        
    def __init__( self,
                  context, name, parent,    # Standard visr component constructor arguments
                  numberOfLoudspeakers,          # The number of point source objects rendered.
                  hrirPositions,            # The directions of the HRTF measurements, given as a Nx3 array
                  hrirData,                 # The HRTF data as 3 Nx2xL matrix, with L as the FIR length.
                  headRadius = 0.0875,      # Head radius, optional. Might be used in a dynamic ITD/ILD individualisation algorithm.
                  useHeadTracking = False,  # Whether head tracking data is provided via a self.headOrientation port.
                  dynamicITD = False,       # Whether ITD delays are calculated and sent via a "delays" port.
                  dynamicILD = False,       # Whether ILD gains are calculated and sent via a "gains" port.
                  hrirInterpolation = False, # HRTF interpolation selection: False: Nearest neighbour, True: Barycentric (3-point) interpolation
                  channelAllocation = False, # Whether to allocate object channels dynamically
                  delays = None,             # Matrix of delays associated with filter dataset. Dimension: # filters * 2
                  ):
        # Call base class (AtomicComponent) constructor
        super( VirtualLoudspeakerController, self ).__init__( context, name, parent )
        self.numberOfLoudspeakers = numberOfLoudspeakers
        self.dynamicITD = dynamicITD
        # %% Define parameter ports
        
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
            
        self.delayOutput = visr.ParameterOutput( "delayOutput", self,
                                                pml.VectorParameterFloat.staticType,
                                                pml.DoubleBufferingProtocol.staticType,
                                                pml.VectorParameterConfig( 2*self.numberOfLoudspeakers) )
        self.delayOutputProtocol = self.delayOutput.protocolOutput()

        if self.dynamicITD:
            if (delays is None) or (delays.ndim != 2) or (delays.shape != (hrirData.shape[0], 2 ) ):
                raise ValueError( 'If the "dynamicITD" option is given, the parameter "delays" must be a #hrirs x 2 matrix.' )

            self.dynamicDelays = np.array(delays, copy=True)
        else:
            self.dynamicDelays = None

#         We are always using the gain oputput matrix to set set the level, even if we do not use a loudness model.
        self.gainOutput = visr.ParameterOutput( "gainOutput", self,
                                               pml.VectorParameterFloat.staticType,
                                               pml.DoubleBufferingProtocol.staticType,
                                               pml.VectorParameterConfig( 2*self.numberOfLoudspeakers) )
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


        #we're using just 2D for the moment
        self.hrirPos = self.hrirPos[:,[0,1]]

#        print("hrirpos 2d"+str(self.hrirPos.shape))

        self.hrirInterpolation = hrirInterpolation
        if self.hrirInterpolation:
            self.lastPosition = np.repeat( [[np.NaN, np.NaN, np.NaN]], self.numberOfLoudspeakers, axis=0 )
            self.hrirLookup = ConvexHull( self.hrirPos )
            self.triplets = np.transpose(self.hrirLookup.points[self.hrirLookup.simplices], axes=(0, 2, 1))
            self.inverted = inv(self.triplets)
            print(self.triplets )
        else:
            self.lastFilters = np.repeat( -1, self.numberOfLoudspeakers, axis=0 )
            self.hrirLookup = KDTree( self.hrirPos )


        # %% Dynamic allocation of objects to channels
        if channelAllocation:
            self.channelAllocator = rbbl.ObjectChannelAllocator( self.numberOfLoudspeakers )
            self.usedChannels = set()
        else:
            self.channelAllocator = None
#            self.headDir = np.repeat( np.array([[1.0,0.0]]), self.numberOfLoudspeakers, axis = 0 )
            self.levels = np.zeros( (self.numberOfLoudspeakers), dtype = np.float32 )

#        self.f = open('srcpAllinone.txt', 'w')

# PRINT 2d convex hull result
        plt.plot(self.hrirPos[self.hrirLookup.vertices,0], self.hrirPos[self.hrirLookup.vertices,1], 'r--', lw=2)
        plt.plot(self.hrirPos[self.hrirLookup.vertices[0],0], self.hrirPos[self.hrirLookup.vertices[0],1], 'bo')
## alternative
#        plt.plot(self.hrirPos[:,0], self.hrirPos[:,1], 'o')
#        for simplex in self.hrirLookup.simplices:
#            plt.plot(self.hrirPos[simplex, 0], self.hrirPos[simplex, 1], 'k-')

        plt.show()
        plt.savefig('convexHull2D.png')

    def process( self ):
#        print("")

##PROFILING                    
##        startTot = time.time()
#        pr = cProfile.Profile()
#        pr.enable()
        
      
            # TODO: This belongs somewhere else in the recompute logic.
            if self.useHeadTracking:
                 if self.trackingInputProtocol.changed():
                     htrack = self.trackingInputProtocol.data()
                     ypr = htrack.orientation
                     
                     # np.negative is to obtain the opposite rotation of the head rotation, i.e. the inverse matrix of head rotation matrix
#                     rotationMatrix = calcRotationMatrix(np.negative(ypr))
                     self.headDir = sph2cart3inp(ypr[0],0,1)[:2]
#                     print(self.headDir.shape)
            # Obtain access to the output arrays
            gainVec = np.array( self.gainOutputProtocol.data(), copy = False )
            delayVec = np.array( self.delayOutputProtocol.data(), copy = False )

            # Set the object for both ears.
            # Note: Incorporate dynamically computed ILD if selected or adjust 
            # the level using an analytic model.
            gainVec[...] = 1

            if self.hrirInterpolation:

#Vectorised replacement
                allGains =  self.inverted @ self.headDir.T
#                print(allGains.shape)
#                allGains = np.matmul( self.inverted, np.transpose(self.headDir) )
                minGains = np.min( allGains, axis = 1 ) # Minimum over last axis
                matchingTriplet = np.argmax( minGains, axis = 0 )
#                print(matchingTriplet)
                #Select the gains for the matching triplets.
                unNormedGains = allGains[matchingTriplet,:]
                gainNorm = np.linalg.norm( unNormedGains, ord=1, axis = -1 )
                normedGains = unNormedGains / gainNorm
#                print(normedGains.shape)
            else:
#                 [ d,indices ] = self.hrirLookup.query( self.headDir, 1, p =2 )
                 dotprod = self.hrirPos @ self.headDir.T
                 indices = np.argmax( dotprod, axis = 0 )
##                 print(found)
                 
            # Retrieve the output gain vector for setting the object level and potentially
            # applying dynamically computed gain adjustement (e.g., nearfield)

# Notice: This spoils the return value of the KD tree query for the non-interpolated case.
#            indices = np.zeros((self.numberOfLoudspeakers,3), dtype = np.int ) 

#
         
#            for chIdx in range(0,self.numberOfLoudspeakers):
#                print("object n: "+str(chIdx))

                # If the source is silent (probably inactive), don't change filters
#            if self.levels[chIdx] >= 1.0e-7:
            if self.hrirInterpolation:
#                       
                _gnorm = normedGains
#                        print(_gnorm)
                
#                        print()
                _indices = self.hrirLookup.simplices[matchingTriplet,:]
#                transp = np.moveaxis(self.hrirs[_indices,:,:], 1,-1 )
#                _interpFilters = np.einsum('ikwj,ij->ikw', transp, _gnorm)
#                print(_indices.shape)
#                print(self.hrirs[_indices,:].shape)
#                print(_gnorm.shape)
                _interpFilters = np.einsum('jkiw,j->ikw', self.hrirs[_indices,...], _gnorm)
#                _interpFilters =  np.matmul(transp, _gnorm.T)
                
                
#                        print(_indices)
      
#                print(self.hrirs[_indices,:,:].shape)

    #            transp = np.moveaxis(self.hrirs[_indices,:,:], [0, 1], [-1, -2] )
                
#                print(transp.shape)
#                print(np.einsum('ikwj,ji->ikw', transp, np.array(_gnorm.T)).shape)

                # TODO: Check whether the explicit np.array() construction is necessary (incurs a copy)
#                print(_interpFilters[0])
#                print(aoerab)                
                for lspIdx in range(0,self.numberOfLoudspeakers):
                        _leftInterpolant = pml.IndexedVectorFloat( lspIdx, _interpFilters[lspIdx,0,:] )
                        _rightInterpolant = pml.IndexedVectorFloat( lspIdx+self.numberOfLoudspeakers, _interpFilters[lspIdx,1,:] )
    #                    _leftInterpolant = pml.IndexedVectorFloat( chIdx, _interpFilters[chIdx,0,:,0] )
    #                    _rightInterpolant = pml.IndexedVectorFloat( chIdx+self.numberOfLoudspeakers, _interpFilters[chIdx,1,:,0] )
                        self.filterOutputProtocol.enqueue( _leftInterpolant )
                        self.filterOutputProtocol.enqueue( _rightInterpolant )
                 
                if self.dynamicITD:
                    # Apparently not working
                    # delays = np.dot(self.dynamicDelays[_indices,:].T,_gnorm)
                    # delayVec[ [chIdx, chIdx + self.numberOfLoudspeakers] ] = delays
                    # Note: matmul() adds a third (singleton) dimension to the result, therefore we have to squeeze it.
#                    print(self.dynamicDelays[_indices,:].shape)
#                    print(np.moveaxis(self.dynamicDelays[_indices,:],1,2).shape)
                    delays = np.squeeze(np.matmul( np.moveaxis(self.dynamicDelays[_indices,:],1,2), _gnorm[...,np.newaxis]),axis=2 )
                    delayVec[0:self.numberOfLoudspeakers] = delays[:,0]
                    delayVec[self.numberOfLoudspeakers:] = delays[:,1]
                else:
                    delayVec[ [lspIdx, lspIdx + self.numberOfLoudspeakers] ] = 0.
        
        #            print(self.hrirs[_indices,:,:])            
#                        print(_interpFilters.shape)            
        #            print(erob)




#
##                         start2 = time.time()
#                        # TODO: the test is not sensible. We would need to test
#                        # both the triplet and the interpolation weights.
#                        if not False: # np.array_equal(self.lastPosition[chIdx],indices[chIdx]):
##                            g = np.dot(self.inverted[tripletnum[chIdx],:,:], self.headDir[chIdx,:])
##                            gnorm = g*1/np.linalg.norm(g,ord=1)
#                            gnorm = normedGains[chIdx,:]
#                            indices = self.hrirLookup.simplices[matchingTriplet[chIdx],:]
#                            # Vectorised filter interpolation code
##                            print(self.hrirs[indices,:,:])
#                            transp = np.moveaxis(self.hrirs[indices,:,:], 0, -1 )
#                           
#                            print(gnorm.shape)
#                            print(self.hrirs[indices,:,:].shape)
#                            print(transp.shape)
#                            interpFilters = np.dot( np.moveaxis(self.hrirs[indices,:,:], 0, -1 ), gnorm )
#                            leftInterpolant = pml.IndexedVectorFloat( chIdx, interpFilters[0,:] )
#                            rightInterpolant = pml.IndexedVectorFloat( chIdx+self.numberOfLoudspeakers, interpFilters[1,:] )
#                            self.filterOutputProtocol.enqueue( leftInterpolant )
#                            self.filterOutputProtocol.enqueue( rightInterpolant )
#                            print(interpFilters.shape)
#                            print(erob)
#
#
#                            self.lastPosition[chIdx] = indices
##                            print("filter out %f sec"%(time.time()-start2))
#                            if self.dynamicITD:
#                                delays = np.dot(self.dynamicDelays[indices,:].T,gnorm)
#                                delayVec[ [chIdx, chIdx + self.numberOfLoudspeakers] ] = delays
##                                print(delays*1000)
##                                print("[%f %f]"%(delayVec[0],delayVec[1]))
#                            else:
#                                delayVec[ [chIdx, chIdx + self.numberOfLoudspeakers] ] = 0.

                                     
            else: # hrirInterpolation == False
                for chIdx in range(0,self.numberOfLoudspeakers):    
                    if self.lastFilters[chIdx] != indices[chIdx]:
                        leftCmd  = pml.IndexedVectorFloat( chIdx,
                                                          self.hrirs[indices[chIdx],0,:])
                        rightCmd = pml.IndexedVectorFloat( chIdx+self.numberOfLoudspeakers,
                                                          self.hrirs[indices[chIdx],1,:])
                        self.filterOutputProtocol.enqueue( leftCmd )
                        self.filterOutputProtocol.enqueue( rightCmd )
                        self.lastFilters[chIdx] = indices[chIdx]
    
                        if self.dynamicITD:
                            delays = self.dynamicDelays[indices[chIdx],:]
                            delayVec[ [chIdx, chIdx + self.numberOfLoudspeakers] ] = delays
                        else:
                            delayVec[ [chIdx, chIdx + self.numberOfLoudspeakers] ] = 0.

            self.gainOutputProtocol.swapBuffers()
            self.delayOutputProtocol.swapBuffers()

            if self.useHeadTracking:
                self.trackingInputProtocol.resetChanged()
          