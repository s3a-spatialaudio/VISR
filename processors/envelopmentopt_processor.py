# -*- coding: utf-8 -*-
"""
Created on Tue Jan 31 17:10:05 2017

@author: Jon Francombe
"""

import numpy
import math
import time

from sys import version_info

if version_info.major <= 2:
    import OSC
else:
    # Use the self-made port for Python 3 (experimental)
    from ..thirdparty import OSC as OSC

from metadapter import SequenceProcessorInterface

# Import from utilities
from .utilities import parse_object_type

# Import from envelopment_functions
from .envelopment_functions import *

def send_status(message,osc_address,host,port):
    c = OSC.OSCClient()
    c.connect((host, port))
    oscmsg = OSC.OSCMessage()
    oscmsg.setAddress(osc_address)

    oscmsg.append(message)

    c.send(oscmsg)


def find_max_spread(objectVector):

    # TODO: Take into account the limits imposed by the available loudspeakers

    # Initialise max spread dictionary
    max_spread = {}

    # Step through the groups calculating the max spread that can be applied
    for g in [3,4,5]:

        #print "Loop index: %i    Group number: %i\n" % (i, g)

        # Only work on the objects in the group
        group_objectVector = [obj for obj in objectVector if obj['group']==g]
        numobjects = len(group_objectVector)

        #print "Group %i has %i objects" % (g,numobjects)

        # Get a list of the object positions
        object_positions = []
        for obj in group_objectVector:
            if obj['type'] == 'plane':
                object_positions.append(obj['direction']['az'])

        # Convert them to +- 180
        for n, value in enumerate(object_positions):
            value = value % 360					# Convert the azimuth to the range 0 to 360
            if value > 180:					# Convert azimuth greater than 180 to negative, so the spread multiplier works
                value = value - 360				# ...

            object_positions[n] = value		# Re-write into object positions

        # NOTE: This bit isn't necessary assuming that the user envelopment modification is off when the optimisation is on
        # Convert the object positions to what they'd be if the spread hadn't already been changed by the envelopment processor
        #orig_object_positions = [(o * 1/current_envelopment) for o in object_positions]


        # Get the maximum positive and negative angles
        max_az = max(object_positions)
        min_az = min(object_positions)

        # Get the absolute maximum
        max_val = max([abs(angle) for angle in [max_az, min_az]])

        # Find the maximum allowed spread
        if max_val > 0:
            max_spread_g = 180 / max_val
        else:
            max_spread_g = 0

        # Add the value to the overall max_spread dictionary
        max_spread[g] = max_spread_g

        # Display some output:
        #print "Group %i:    Greatest azimuth: %2.2f.   Therefore, greatest spread: %2.2f" % (g, max_val, max_spread[g])

    # Return the max_spread dictionary, which contains a group number followed by the maximum spread that can be applied to that group
    return max_spread


#class EnvelopmentOptProcessor(MetadataProcessorInterface):
class EnvelopmentOptProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        #MetadataProcessorInterface.__init__(self, arguments)
        SequenceProcessorInterface.__init__(self, arguments)

		# Set up options
        self.options = {}
        #self.options['numsteps'] = 10     # This is the number of steps that are allowed between the current value and the maximum value (i.e. if max spread is 10, the stepsize will be 1)
        self.options['tolerance'] = 10     # The acceptable tolerance either side of the target level that the predicted level should be
        self.options['period'] = 10        # This is how many metadapter periods to wait before doing anything

        # Get arguments
        if 'on' in arguments.attrib:
            self.on = int(arguments.attrib['on'])
        else:
            self.on = 1   # DEFAULT FOR NOW IS ON

        if 'verbose' in arguments.attrib:
            self.verbose = int(arguments.attrib['verbose'])
        else:
            self.verbose = 1 # DEFAULT FOR NOW IS VERBOSE

        if 'target' in arguments.attrib:
            self.target_env = float(arguments.attrib['target'])
        else:
            self.target_env = 100

        if 'tolerance' in arguments.attrib:
            self.options['tolerance'] = float(arguments.attrib['tolerance'])

        if 'period' in arguments.attrib:
            self.options['period'] = float(arguments.attrib['period'])

        # Initialise predicted envelopment
        self.predicted_env = 100

        # This flag is used so that the envelopment is only checked and altered when a prediction is recieved
        self.check = False

        # Set up parameter vectors
        self.params = []                                                                        													# Index Name
        #                                                                                       													--------------------------
        self.params.append(list(numpy.linspace(start=3.0, stop=-1.0, num=201)))                 													# 0     Clear speech level
        self.params.append(list(numpy.linspace(start=0.0, stop=30.0, num=201)))                 													# 1     Clear speech elevation

        self.params.append(list(numpy.linspace(start=-0.05, stop=0.75, num=201)))              														# 2     Foreground action level
        self.params.append(list(numpy.linspace(start=0.0, stop=1.0, num=101)) + list(numpy.linspace(start=1.0, stop=5.0, num=101)))                 # 3     Foreground action spread
        del(self.params[3][101])

        self.params.append(list(numpy.linspace(start=-1.5, stop=1.8, num=201)))              														# 4     Background action level
        self.params.append(list(numpy.linspace(start=0.0, stop=1.0, num=101)) + list(numpy.linspace(start=1.0, stop=5.0, num=101)))                 # 5     Background action spread
        del(self.params[5][101])

        self.params.append(list(numpy.linspace(start=-4.3, stop=8.0, num=201)))              														# 6     Reverb + effects level
        self.params.append(list(numpy.linspace(start=0.0, stop=1.0, num=101)) + list(numpy.linspace(start=1.0, stop=5.0, num=101)))                 # 7     Reverb + effects spread
        del(self.params[7][101])

        self.params.append(list(numpy.linspace(start=0.6, stop=0.4, num=201)))                 														# 8     Overall level
        self.params.append(list(numpy.linspace(start=-2.75, stop=1.0, num=201)))                 													# 9     Overall LF boost
        self.params.append(list(numpy.linspace(start=0.0, stop=1.5, num=201)))                 														# 10    Overall HF boost

        self.params.append(list(numpy.linspace(start=-2, stop=2, num=201)))                                                                         # 11    Music level
        self.params.append(list(numpy.linspace(start=0.0, stop=1.0, num=101)) + list(numpy.linspace(start=1.0, stop=5.0, num=101)))                 # 12    Music spread
        del (self.params[12][101])

        # Initialise the head position of the vector
        self.head = 100

        self.count = 0      # Start a counter for the number of modifications that have been made

        self.periods = 0    # Count the number of metadapter periods

    def processObjectVector(self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.

        # Each period, increment the count
        self.periods += 1
        if self.periods > self.options['period']:
            self.periods = 1

        # Temporary debug variable:
        debug = 1

        if self.on:

            if self.check is True and self.periods == self.options['period']:  # If it's a checking cycle (i.e. a prediction has been recieved) and there have been the right number of periods

                # Calculate the difference between target and predicted envelopment
                diff = self.target_env - self.predicted_env

                # Write status message
                if self.predicted_env < self.target_env + self.options['tolerance']:
                    status_message = "Checked envelopment. Prediction (%2.2f) is lower than target and tolerance (%2.2f to %2.2f)." % ( self.predicted_env, self.target_env - self.options['tolerance'], self.target_env + self.options['tolerance'] )

                elif self.predicted_env > self.target_env - self.options['tolerance']:
                    status_message = "Checked envelopment. Prediction (%2.2f) is higher than target and tolerance (%2.2f to %2.2f)." % (self.predicted_env, self.target_env - self.options['tolerance'], self.target_env + self.options['tolerance'])

                # See if it's within the tolerance
                if abs(diff) > self.options['tolerance']: 	# Predicted envelopment is outside of the tolerance
                    if self.verbose:
                        print(status_message)

                    # Move the vector head by diff
                    self.head = self.head + int(diff)

                    # Warn if a limit is reached
                    if self.head > 200:
                        print( "No further envelopment increase can be made" )
                        self.head = 200

                        status_message = status_message + " Can't make more enveloping."

                    elif self.head < 0:
                        print( "No further envelopment decrease can be made" )
                        self.head = 0

                        status_message = status_message + " Can't make less enveloping."

                    else:   # a limit hasn't been reached
                        status_message = status_message + " Doing envelopment processing."

                    if debug:
                        # Describe everything that's happening:
                        print( "Current head: {:i}".format(self.head) )

                else:  # Predicted envelopment is inside of the tolerance

                    status_message = "Checked envelopment. Prediction (%2.2f) is inside target and tolerance (%2.2f to %2.2f). Doing nothing." % ( self.predicted_env, self.target_env - self.options['tolerance'], self.target_env + self.options['tolerance'] )

                    if self.verbose:
                        print("Predicted envelopment is inside of the tolerance")
                        # Don't need to do any processing :)

                # Reset the check variable (so the processor won't try to change again until it recieves another prediction)
                self.check = False

                # Send status message via UDP
                send_status(status_message,'/envopt_status','127.0.0.1',4501)


            # Do the processing (if on, not just if checking)
            for i, obj in enumerate(objectVector):

                #print ('Object: %i' % i)
                #print obj
                #print

                current_group = GetCurrentGroup(obj)

                # Parse the object type to see how it's position is specified
                objtype, pos = parse_object_type(obj)

                if current_group == 1 or current_group == 'speech':			# Clear speech

                    # Set the level
                    obj['level'] = float(obj['level']) * dB2lin(self.params[0][self.head])

                    send_osc(self.params[0][self.head],'clearspeechlevel','127.0.0.1',4556)

                    # Set the elevation
                    new_el = self.params[1][self.head]

                    #If it's not az, el, r, then convert...
                    if pos[2] is not 'az':
                        p1, p2, p3 = sphDeg2cart(0, new_el, 1)
                    else:
                        p1 = 0
                        p2 = new_el
                        p3 = 1

                    obj[pos[0]] = {pos[1]: p1, pos[2]: p2, pos[3]: p3}

                    #orig_az = float(obj['direction']['az'])
                    #orig_r = float(obj['direction']['refdist'])
                    #obj['direction'] =  { 'az': orig_az, 'el': self.params[1][self.head], 'refdist': orig_r }


                elif current_group == 2 or current_group == 'music':          # Non-diegetic music

                    # Set the level
                    obj['level'] = float(obj['level']) * dB2lin(self.params[11][self.head])
                    send_osc(self.params[11][self.head],'musiclevel','127.0.0.1',4556)

                    # Set the spread
                    obj = dospread(obj, self.params[12][self.head])

                elif current_group == 3 or current_group == 'foreground':  # Foreground action
                    # Set the level
                    obj['level'] = float(obj['level']) * dB2lin(self.params[2][self.head])
                    send_osc(self.params[2][self.head],'foregroundlevel','127.0.0.1',4556)

                    # Set the spread
                    obj = dospread(obj,self.params[3][self.head])

                elif current_group == 4 or current_group == 'background':  # Background sounds
                    # Set the level
                    obj['level'] = float(obj['level']) * dB2lin(self.params[4][self.head])
                    send_osc(self.params[4][self.head],'backgroundlevel','127.0.0.1',4556)

                    # Set the spread
                    obj = dospread(obj,self.params[5][self.head])

                elif current_group == 5 or current_group == 'reverb':  # Reverb & effects
                    # Set the level
                    obj['level'] = float(obj['level']) * dB2lin(self.params[6][self.head])
                    send_osc(self.params[6][self.head],'reverblevel','127.0.0.1',4556)

                    # Set the spread
                    obj = dospread(obj,self.params[7][self.head])

                    # Overall processing
                # Level
                obj['level'] = float(obj['level']) * dB2lin(self.params[8][self.head])

                send_osc(self.params[8][self.head], 'overalllevel', '127.0.0.1', 4556)


                # EQ
                eq_low = {u'type': u'lowshelf', u'f': 200.0, u'q': 1.0, u'gain': self.params[9][self.head]}
                eq_high = {u'type': u'highshelf', u'f': 2000.0, u'q': 1.0, u'gain': self.params[10][self.head]}
                obj[u'eq'] = [eq_low, eq_high]

                send_osc(self.params[9][self.head], 'lflevel', '127.0.0.1', 4556)
                send_osc(self.params[10][self.head], 'hflevel', '127.0.0.1', 4556)


                '''
                # How much spread is allowed?
                #max_spread = find_max_spread(objectVector)     # Call into this using max_spread[group_number], e.g. max_spread[3]
                max_spread = 1.0
                '''


        else: # Processor is not on

            if False: # Turned this message off for now
                print("Envelopment optimisation processor is off")



        # Return the object vector:
        return objectVector




    # GET INPUTS:
    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""

        onstatus = ['off','on']

        if key == "predicted_env":
            self.predicted_env = valueList[0]
            self.check = True
            #if self.verbose:
            #    print "Predicted envelopment: %2.2f" % (self.predicted_env)

        elif key == "target_env":
            self.target_env = valueList[0]
            if self.verbose:
                print ("Target envelopent set to: {:2.2f}".format(self.target_env))

        elif key == "tolerance":
            self.options['tolerance'] = valueList[0]
            if self.verbose:
                print ("EnvOpt tolerance set to: {:2.2f}".format(self.options['tolerance']))

        #elif key == "envelopment":
            #self.user_env = valueList[0]

        elif key == "on":
            if valueList[0] == 0 or valueList[0] == 1:
                self.on = valueList[0]
                if self.verbose:
                    print("Envelopment optimisation processor is {}".format(onstatus[self.on]))
            else:
                raise KeyError( "Command \"on\" must be 0 or 1" )

        elif key == "reset":
            self.head = 100
            print("Envelopment optimisation: mix reset to default level")

        elif key == "verbose":
            if valueList[0] == 0 or valueList[0] == 1:
                self.verbose = valueList[0]
                if self.verbose:
                    print("Envelopment optimisation is {}".format(['not verbose','verbose'][self.verbose]))

        else:
            raise KeyError( "Inputs allowed: predicted_env (float, e.g. from model), target_env (float), on (1 or 0), or reset (any message)" )
