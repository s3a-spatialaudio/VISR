# -*- coding: utf-8 -*-
"""
Created on Fri Sept 2 2016

@author: JF
"""

import math
from distutils.util import strtobool

#from metadata_processor_interface import MetadataProcessorInterface
from metadapter import SequenceProcessorInterface

from copy import deepcopy

def lin2dB( linVal ):
    return 20.0 * math.log10( linVal )
    
def dB2lin( dbVal ):
    return math.pow( 10.0, 0.05 * dbVal )

#class SoloProcessor(MetadataProcessorInterface):
class SoloProcessor(SequenceProcessorInterface):
    
    def __init__(self, arguments ):
        #MetadataProcessorInterface.__init__(self, arguments)
        SequenceProcessorInterface.__init__(self, arguments)
        #self.objectLookup = {}
        
        self.objectLookup = {}

        
        # Get the groups which are soloed
        soloStr = arguments.attrib['initialSolo']
        self.solo = soloStr.split()      
        
        # Get active 
        self.active = strtobool( arguments.attrib['active'] )

  		# Get the groups (names and object IDs)
        for groupNode in arguments.iter( 'group' ):
            groupName = groupNode.attrib['name']
            objsStr = groupNode.attrib['objectIds']
            #initialSolo = [int(groupNode.attrib['initialSolo'])]
            objIds = [int(s) for s in objsStr.split() if s.isdigit()]
            for objId in objIds:
                self.objectLookup[objId] = groupName
            
    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.
        objectVectorNew = deepcopy(objectVector)    

        # For debugging, turn on safety check
        do_safe = 1
                    
        # Preallocate a flag that will be used for checking if anything was done
        flag = 0

        # Mute value - this is how much the object will be muted by
        mutecheck = 200
        mutevalue = 240

        
        for obj in objectVectorNew:
                        
            # If the current level is above -mutecheck dB, mute it 
            if obj['level'] > (1 * dB2lin( -mutecheck ) ):
                obj['level'] = float( obj['level'] ) * dB2lin( -mutevalue )          
            
            
                
            # Get the object ID
            id = int( obj['id'] )
                        
            # See if that ID is one that should be soloed
            
            
            if self.objectLookup[id] in self.solo:
                
                flag = 1 # Set a flag so that can check if no groups have been matched
                                       
                # If it's not already boosted, boost it
                if obj['level'] < (1 * dB2lin( -mutecheck ) ):
                    obj['level'] = float( obj['level'] ) * dB2lin( mutevalue )
                                     
            
            # Safety check:
            if do_safe:
                if float ( obj['level'] ) > (1 * dB2lin( 24 )):
                    print( "Warning, too loud. Setting to -24 dB.")
                    obj['level'] = 1 * dB2lin( -24 )         
            
            
            
        # For loop (each object) ends here        
        # See if the flag's been set. If it hasn't, then all objects need to be unmuted.
        if not flag:
            for obj in objectVectorNew:
                # If it's not already boosted, boost it
                if obj['level'] < (1 * dB2lin( -mutecheck ) ):
                    obj['level'] = float( obj['level'] ) * dB2lin( mutevalue )   
                # Safety check:
                if do_safe:
                    if float ( obj['level'] ) > (1 * dB2lin( 24 )):
                        print( "Warning, too loud. Setting to -24 dB.")
                        obj['level'] = 1 * dB2lin( -24 )  
                     
        return objectVectorNew
        
    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        #if (len( valueList ) != 1) or (not isinstance(valueList[0], float) ):
            #raise KeyError( "SoloProcessor supports only commands consisting of a single float value" )
        #if not key in self.solo:
        #    raise KeyError( "SoloProcessor: The command name does not correspond to a known group." )
        
        if (key == "active"):
            if len( valueList ) != 1:
                raise KeyError( "LimitsProcessor command \"active\" must contain 1 value." )
            self.active = bool( valueList[0] )
            
        if (key == "solo"):
            #print valueList
            solo_in = valueList #[0]
            self.solo = solo_in
            #print self.solo
