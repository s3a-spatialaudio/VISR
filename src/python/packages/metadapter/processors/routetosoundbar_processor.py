# -*- coding: utf-8 -*-
"""
Created on Fri Feb 03 2017

@author: Jon Francombe
"""

#import numpy
import math

#from metadata_processor_interface import MetadataProcessorInterface
# Metadapter2 version:
from metadapter import SequenceProcessorInterface


class RouteToSoundbarProcessor(SequenceProcessorInterface):
    
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)

        if 'on' in arguments.attrib:
            self.on = int(arguments.attrib['on'])
        else:
            self.on = 0
        
        
        if 'soundbarchannel' in arguments.attrib:
            self.soundbarchannel = int(arguments.attrib['soundbarchannel'])
        else:
            self.soundbarchannel = 'None'
            print( "WARNING: No soundbar channel specified. VBAP array will be used" )

        if 'initialobjects' in arguments.attrib:
            objsStr = arguments.attrib['initialobjects']
            self.objects = [int(s) for s in objsStr.split() if s.isdigit()]
        else:
            self.objects = []
                    
    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.
        
        if self.on and self.soundbarchannel is not 'None':     # If the processing is on and there's a specified soundbar channel   
            
            for obj in objectVector:
                
                if int(obj['id']) in self.objects: # If it's one of the soundbar objects, move it to the soundbar
                    
                                       
                    obj['type'] = 'channel'
                    obj['outputChannels'] = self.soundbarchannel         

        
          
        # Note: this has to be returned otherwise no objects are replayed    
        return objectVector
        


    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        
        if key == "on":
            if valueList[0] == 0 or valueList[0] == 1:
                self.on = valueList[0]
                print( "Soundbar routing processor on/off: %i" % self.on )
            else:
                raise KeyError( "Command \"on\" must be 0 or 1" )
    
                                    
        else:  
            raise KeyError( "MDOProcessor supports only the parameter set commands \"on\"")
        
        
        #if (key != "envelopment") or (len( valueList ) != 1) or (not isinstance(valueList[0], float) ):
        #    raise KeyError( "MDOProcessor supports only the parameter set command \"envelopment\" consisting of a single float value" )
        #self.env = valueList[0]
        #print( "Envelopment set at %2.2f" % (self.env) )
