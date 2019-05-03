# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:10:05 2016

@author: Jon Francombe
"""

import numpy
import math

from metadapter import SequenceProcessorInterface


# Import from utilities
from .utilities import parse_object_type
from .utilities import print_empty

# Import from envelopment_functions
from .envelopment_functions import *




def send_osc(string,osc_address,host,port):
    c = OSC.OSCClient()
    c.connect((host, port))
    oscmsg = OSC.OSCMessage()
    oscmsg.setAddress(osc_address)

    oscmsg.append(string)

    c.send(oscmsg)

def SendValuesToMax(self):

    # Send decibel values as osc messages
    send_osc(3 * self.ni, '/cat0', '127.0.0.1', 4557)
    send_osc(0 * self.ni, '/cat1', '127.0.0.1', 4557)
    send_osc(-12 * self.ni, '/cat2', '127.0.0.1', 4557)
    send_osc(-48 * self.ni, '/cat3', '127.0.0.1', 4557)


def MakeStrings(objectVector):

    cat_string = ['Objects in cat 0: ','Objects in cat 1: ','Objects in cat 2: ','Objects in cat 3: ']

    for obj in objectVector:

        # Set the NI
        if 'narrative_importance' in obj:
            ni_level = int(obj['narrative_importance'])
        else:
            ni_level = 1

        # Look for an object name
        if 'content_label' in obj:
            cat_string[ni_level] = cat_string[ni_level] + obj['content_label'] + ', '
        else:
            #object_string = object_string + '(unlabelled object, ID: ' + str(obj['id']) + ', '
            cat_string[ni_level] = cat_string[ni_level] + str(obj['id']) + ', '


    for n, string in enumerate(cat_string):

        cat_string[n] = cat_string[n][:-2]   # Remove the last ', '

        print(cat_string[n])

        oscaddress = "/string%i" % int(n)

        # Send OSC message to interface
        send_osc(cat_string[n],oscaddress,'127.0.0.1',4557)


#class NarrativeImportanceProcessor(MetadataProcessorInterface):
class NarrativeImportanceProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        #MetadataProcessorInterface.__init__(self, arguments)
        SequenceProcessorInterface.__init__(self, arguments)

        # Get arguments:
        if 'on' in arguments.attrib:
            self.on = int(arguments.attrib['on'])
        else:
            self.on = 1   # DEFAULT FOR NOW IS ON

        if 'narrativeimportance' in arguments.attrib:
            self.ni = float(arguments.attrib['narrativeimportance'])
        else:
            self.ni = 1.0

        if 'verbose' in arguments.attrib:
            self.verbose = int(arguments.attrib['verbose'])
        else:
            self.verbose = 1 # DEFAULT FOR NOW IS VERBOSE

        if 'lock' in arguments.attrib:
            self.dolock = int(arguments.attrib['lock'])
        else:
            self.dolock = 0 # Default has the position lock off


        SendValuesToMax(self)

        self.sendstrings = 0
        self.sendlock = 2



    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.

        if self.on:

            for i, obj in enumerate(objectVector):

                # Parse the object type to see how it's position is specified
                objtype, pos = parse_object_type(obj)

                # Get the narrative importance. If it's not specified, then it's a 1 (there'll be no change)
                if 'narrative_importance' in obj:
                    ni_level = int(obj['narrative_importance'])
                else:
                    ni_level = 1

                if ni_level == 0:

                    obj['level'] = float(obj['level']) * dB2lin((3 * self.ni))

                elif ni_level == 1:

                    if False:
                        print()

                elif ni_level == 2:

                    obj['level'] = float(obj['level']) * dB2lin((-12 * self.ni))

                elif ni_level == 3:

                    obj['level'] = float(obj['level']) * dB2lin((-48 * self.ni))

                # Lock position (but only if NI is set at 0.75 or above
                if self.dolock and self.ni >= 0.75 and 'lock_position' in obj:

                    if obj['lock_position'] != 'off':
                        lock_position = float(obj['lock_position'])
                        #print('Locking object %i at %f' % (int(obj['id']), lock_position))

                        # Get current position
                        thetype, pos = parse_object_type(obj)

                        # If necessary, convert to degrees
                        if pos[1] != 'az':  # If it's not in degrees (i.e. it's cartesian), convert the lock position to cartesian
                            new1, new2, new3 = sphDeg2cart(lock_position,0,1)
                        else:
                            new1 = lock_position
                            new2 = 0
                            new3 = 1

                        # Set locked position
                        obj[pos[0]] = {pos[1]: new1, pos[2]: new2, pos[3]: new3}

            # This sends object category strings to the user interface
            if self.sendstrings:
                MakeStrings(objectVector)
                self.sendstrings = 0

            # This makes object-lock strings and sends them to the user interface
            if self.dolock and self.sendlock == 1 and 'lock_position' in objectVector[1]:

                lock_string = 'These objects have fixed positions: '

                for obj in objectVector:
                    if 'lock_position' in obj and obj['lock_position'] != 'off':

                        lock_position = float(obj['lock_position'])

                        # Look for an object name
                        if 'content_label' in obj:
                            lock_string = lock_string + obj['content_label'] + ' (' + str(lock_position) + ' deg)' + ', '  # °
                        else:
                            print( 'here2' )
                            lock_string = lock_string + str(obj['id']) + ' (' + str(lock_position) + ' deg)' + ', '

                lock_string = lock_string[:-2]  # Remove the last ', '
                print( lock_string )

                # Send OSC message to interface
                send_osc(lock_string, "/lockstring", '127.0.0.1', 4557)

                self.sendlock = 0

            elif self.sendlock == 2:    # Switched into immersive mode

                # Send empty OSC message to interface
                send_osc('', "/lockstring", '127.0.0.1', 4557)
                self.sendlock = 0

        return objectVector

    def setParameter(self, key, valueList):
        """ Set the parameter to a given value."""

        if key == "narrativeimportance":

            newni = valueList[0]

            if newni >= 0.75 and self.ni < 0.75:            # Switched into NI mode so send a string with object lock details
                print( 'switched into narrative mode' )
                self.sendlock = 1
            elif newni < 0.75 and self.ni >= 0.75:
                print( 'switched into immersive mode' )       # Switched into immersive mode, so send an empty string
                self.sendlock = 2

            self.ni = newni
            SendValuesToMax(self)
            if self.verbose and self.on:
                print("Narrative level set at %2.2f" % self.ni)

        elif key == "on":
            if valueList[0] == 0 or valueList[0] == 1:
                self.on = valueList[0]
                if self.verbose:
                    print("Narrative importance processor is %s" % ['off', 'on'][self.on])
            else:
                raise KeyError("Command \"on\" must be 0 or 1")

        elif key == "sendstrings":
            self.sendstrings = 1

        elif key == "lock":
            self.dolock = valueList[0]
            if self.dolock == 0:
                self.sendlock = 2
            elif self.dolock == 1:
                self.sendlock = 1

        else:
            raise KeyError("Narrative importance processor supports only the parameter set commands \"on\", \"narrativeimportance\", \"sendstrings\", and \"lock\"")
