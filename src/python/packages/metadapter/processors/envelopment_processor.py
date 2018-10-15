# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:10:05 2016

@author: Jon Francombe
"""

import numpy
import math

#from metadata_processor_interface import MetadataProcessorInterface
from metadapter import SequenceProcessorInterface


# Import from utilities
from .utilities import parse_object_type
from .utilities import print_empty

# Import from envelopment_functions
from .envelopment_functions import *


#class EnvelopmentProcessor(MetadataProcessorInterface):
class EnvelopmentProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        #MetadataProcessorInterface.__init__(self, arguments)
        SequenceProcessorInterface.__init__(self, arguments)

        # Get arguments:
        if 'on' in arguments.attrib:
            self.on = int(arguments.attrib['on'])
        else:
            self.on = 1   # DEFAULT FOR NOW IS ON

        if 'envelopment' in arguments.attrib:
            self.env = float(arguments.attrib['envelopment'])
        else:
            self.env = 1.0

        if 'verbose' in arguments.attrib:
            self.verbose = int(arguments.attrib['verbose'])
        else:
            self.verbose = 1 # DEFAULT FOR NOW IS VERBOSE

    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.

        # Flags for debugging (temporary):
        debug = False
        printobjectvector = False

        if self.on:

            for i, obj in enumerate(objectVector):

                #print("Object ID: %i" % int(obj['id']))

                # Parse the object type to see how it's position is specified
                objtype, pos = parse_object_type(obj)

                current_group = GetCurrentGroup(obj)

                #print("Object ID: %i.   Object group: %s.   Object type: %s.    Object pos: %s." % (int(obj['id']),str(current_group),type,pos) )

                # This ignores all but one group:
                #if current_group != 5:
                #    continue


                if current_group == 1 or current_group == 'speech':			# Clear speech

                    # Set the level
                    obj = dolevel(obj,1.2,0.7,self.env,'clearspeechlevel')

                    # Set the position (central but elevation changes)
                    median_height = 17 	# Degrees. From results

                    # Determine the new elevation
                    el = median_height * self.env 	# New elevation (scaled from 0 to median_height)

                    # If it's not az, el, r, then convert...
                    if pos[2] is not 'az':
                        p1,p2,p3 = sphDeg2cart(0,el,1)
                    else:
                        p1 = 0
                        p2 = el
                        p3 = 1

                    # Add the metadata (regardless of object type)
                    #newdic = {pos[1]: p1, pos[2]: p2, pos[3]: p3}
                    #print ("Envelopment optimisation processor is about to try this: %s" % newdic)
                    obj[pos[0]] = {pos[1]: p1, pos[2]: p2, pos[3]: p3}

                    # OLD VERSION:
                    #if obj['type'] == 'plane':
                    #    obj['direction'] = { 'az': 0, 'el': el, 'refdist': 1 }

                elif current_group == 2 or current_group == 'music':          # Non-diegetic music
                    # Set the level
                    obj = dolevel(obj, -0.4, 1.5, self.env, 'musiclevel')

                    # Set the spread
                    obj = dospread(obj, self.env)

                elif current_group == 3 or current_group == 'foreground':  # Foreground action
                    # Set the level
                    obj = dolevel(obj, -0.05, 0.75, self.env, 'foregroundlevel')

                    # Set the spread
                    obj = dospread(obj, self.env)

                elif current_group == 4 or current_group == 'background':  # Background sounds

                    # Set the level
                    obj = dolevel(obj, -1.5, 1.8, self.env, 'backgroundlevel')

                    # Set the spread
                    obj = dospread(obj, self.env)

                elif current_group == 5 or current_group == 'reverb':  # Reverb & effects

                    # Set the level
                    obj = dolevel(obj, -4.3, 8.0, self.env, 'reverblevel')

                    # Set the spread
                    obj = dospread(obj, self.env)

                # else:
                # Do nothing, as the group has no processes attached

                ### Processing for all objects

                # Overall level
                obj = dolevel(obj,0.6,0.4,self.env, 'overalllevel')

                # Overall EQ
                obj = doeq(obj,-2.75,1.0,0.0,1.5,self.env)

            if printobjectvector is True:
                print (objectVector)

        return objectVector

    def setParameter(self, key, valueList):
        """ Set the parameter to a given value."""

        onstatus = ['off', 'on']

        if key == "envelopment":
            self.env = valueList[0]
            if self.verbose and self.on:
                print("Envelopment set at {:2.2f}".format(self.env))

        elif key == "on":
            if valueList[0] == 0 or valueList[0] == 1:
                self.on = valueList[0]
                if self.verbose:
                    print("Envelopment processor is {}".format(onstatus[self.on]))
            else:
                raise KeyError("Command \"on\" must be 0 or 1")

        else:
            raise KeyError("Envelopment processor supports only the parameter set commands \"on\" and \"envelopment\"")
