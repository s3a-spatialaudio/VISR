# -*- coding: utf-8 -*-
"""
Created on Fri Feb 03 2017

@author: Jon Francombe
"""

import numpy
import math

from sys import version_info

if version_info.major <= 2:
    import OSC
else:
    # Use the self-made port for Python 3 (experimental)
    from ..thirdparty import OSC as OSC

#from metadata_processor_interface import MetadataProcessorInterface
# Metadapter2 version:
from metadapter import SequenceProcessorInterface


# FUNCTION to convert radians to degrees
def rad2deg( w ):
    return 180.0/math.pi * w

# FUNCTION to convert degrees to radians
def deg2rad( w ):
    return w / 180.0*math.pi

# FUNCTION to convert spherical degrees to cartesian
def sphDeg2cart(az,el,r):

    az = deg2rad(az)
    el = deg2rad(el)

    z = r * math.sin(el)
    rcoselev = r * math.cos(el)
    x = rcoselev * math.cos(az)
    y = rcoselev * math.sin(az)

    return x, y, z

# FUNCTION to convert cartesian to degrees
def cart2sphDeg(x,y,z):
    XsqPlusYsq = x**2 + y**2
    r = math.sqrt(XsqPlusYsq + z**2)               # r
    elev = math.atan2(z,math.sqrt(XsqPlusYsq))     # theta
    az = math.atan2(y,x)                           # phi
    return r, rad2deg(az), rad2deg(elev)

# FUNCTIONS to convert between dB and linear
def lin2dB( linVal ):
    return 20.0 * math.log10( linVal )

def dB2lin( dbVal ):
    return math.pow( 10.0, 0.05 * dbVal )

def send_loudspeaker_content(string,osc_address,host,port):
    c = OSC.OSCClient()
    c.connect((host, port))
    oscmsg = OSC.OSCMessage()
    oscmsg.setAddress(osc_address)

    oscmsg.append(string)

    c.send(oscmsg)



# FUNCTION to parse room description .json file (specified by a key input to the metadapter)
def Parse_Room_Description(filename):

    import json

    # Set up room object dictionary
    room_obj_list = ['None','Bed','Books','Ceiling','Chair','Floor','Furniture','Objects','Picture','Sofa','Table','TV','Unknown','Wall','Window','Any']
    room_obj_dict = {key: value for (value, key) in enumerate(room_obj_list)}

    #print( room_obj_dict['chair'] )

    if filename == 'None':
        print( "MDO processor: no room description file (.json) specified" )
        return None

    # Read the file
    with open(filename) as data_file:
        data = json.load(data_file)
    #r = list(data)

    # Preallocate a list
    room_objects = []

    print( "MDO processor: found some room objects (in %s):" % filename )
    for x in list(data):


        if 'Category' in data[x]:

            object_category = data[x]['Category'].encode('ascii')

            if object_category not in room_obj_list:
                object_category = "Unknown"

        else: # No category specified

            object_category = "Unknown"



        room_objects.append( dict(
                                name=x.encode('ascii'),
                                category=object_category,
                                room_obj_num=room_obj_dict[object_category],
                                x=data[x]['Centre_x'],
                                y=data[x]['Centre_y'],
                                z=data[x]['Centre_z'] ) )

    print('')
    print( "Found %i room objects:" % len(room_objects) )

    for i, ro in enumerate(room_objects):
        print( "    %i. Name: %s.    Category: %s (category number %i)" % (i+1, ro['name'], ro['category'], float(ro['room_obj_num'])))

    print('')

    return room_objects


# FUNCTION to parse loudspeaker config .xml file
def Parse_Loudspeaker_Config(filename):

    if filename == 'None':
        print( "MDO processor: no loudspeaker config file (.xml) specified" )
        return None

    import xml.etree.ElementTree as ET

    # Read the file
    tree = ET.parse(filename)
    root = tree.getroot()


    # Preallocate a list of loudspeakers
    loudspeakers = []

    for child in root:
        if child.tag == "loudspeaker": # found a loudspeaker

            if 'name' in child.attrib:
                name = child.attrib['name']
            else:
                name = 'no name' 	# Unknown name is set as 'no name'

            if 'mdo' in child.attrib: 			# If the MDO attribute exists
                mdo = int(child.attrib['mdo'])  # ...see what it is
            else:  								# Otherwise, it doesn't exist
                mdo = 0 						# ...set MDO to zero

            if 'quality' in child.attrib:
                quality = child.attrib['quality']
            else:
                quality = 'unknown' 	# Unknown quality is set as unknown

            if 'type' in child.attrib:
                type = child.attrib['type']
            else:
                type = 'unknown'

            if 'switch' in child.attrib:
                switch = int(child.attrib['switch'])
            else:
                switch = 1

            if 'function' in child.attrib:
                function = child.attrib['function']
            else:
                function = 'unknown'

            id = int(child.attrib['id'])
            channel = int(child.attrib['channel'])


            # Get the loudspeaker position
            for lschild in child:

                #print( lschild.attrib )

                if lschild.tag == "polar":
                    x,y,z = sphDeg2cart(float(lschild.attrib['az']),float(lschild.attrib['el']),float(lschild.attrib['r']))

                elif lschild.tag == "cart":
                    x,y,z = float(lschild.attrib['x']),float(lschild.attrib['y']),float(lschild.attrib['z'])

                #print( x,y,z )


            # Compile the loudspeaker list of dictionaries
            loudspeakers.append( dict(name=name, id=id, channel=channel, mdo=mdo, quality=quality, type=type, x=x, y=y, z=z, switch=switch, function=function) )

    #for l in loudspeakers:
        #print( l )

            #print "Found a loudspeaker. MDO: %i" % int(mdo)


        # See if it should be used for MDO.
        # If so, get its position, quality, type, channel

    print('')
    print("Found %i loudspeakers:" % len(loudspeakers))

    for i, ls in enumerate(loudspeakers):
        print('    %i. Name: %s.    Type: %s.    Quality: %s.    MDO loudspeaker: %s.    x: %2.2f  y: %2.2f  z: %2.2f' % (i+1, ls['name'], ls['type'], ls['quality'], ['no','yes'][ls['mdo']], ls['x'], ls['y'], ls['z']))

    print('')


    return loudspeakers


# FUNCTION: find nearest loudspeaker (return the channel number)
def Find_Nearest_Loudspeaker(point,loudspeakers):
    norm_dist = True
    if norm_dist == True:
        #print point
        mag = math.sqrt(point[0]**2 + point[1]**2 + point[2]**2)
        point = [n/mag for n in point]
        #print point

    if type(point) is list:
        # Convert list to tuple
        point = tuple(point)



    from scipy.spatial import distance

    dist = []
    az_diff = []
    ls_az_list = []

    for l in loudspeakers:

        # Get the loudspeaker position
        if norm_dist == True:
            l_norm = [l['x'], l['y'], l['z']]
            mag = math.sqrt(l_norm[0]**2 + l_norm[1]**2 + l_norm[2]**2)
            l_norm = [n/mag for n in l_norm]
            ls_coords = (l_norm[0], l_norm[1], l_norm[2])
        else:
            ls_coords = (l['x'], l['y'], l['z'])

        # Get speaker azimuths
        ls_az = math.atan2(l['y'],l['x'])%(2*math.pi)
        ls_az_list.append(ls_az*180/math.pi)
        #print ls_az*180/math.pi

        # Get point azimuth
        point_az = math.atan2(point[1],point[0])%(2*math.pi)

        # Calculate the euclidean distance
        dist.append(distance.euclidean(point,ls_coords))

        # Calculate angle differences
        az_diff.append(abs(ls_az - point_az))

        # Print
        #print("Loudspeaker %i (%s) position: %s" %(int(l['id']),l['name'],ls_coords))


    if len(dist) == 0:
        closest_channel = 'None'

    else:

        # Find the loudspeaker with the smallest euclidean distance
        closest = dist.index(min(dist)) # For now, just get the first minimum distance

        closest_az = az_diff.index(min(az_diff))
        #print "Target azimuth:"
        #print point_az*180/math.pi

        #print "Closest LS azimuth:"
        #print ls_az_list[closest_az]

        # Get the ID from the closest loudspeaker
        closest_channel = loudspeakers[closest]['id']

    # Return the closest channel number
    return closest_channel


# FUNCTION: list acceptable loudspeakers
def List_Acceptable_Loudspeakers(loudspeakers,criterion,value,force_mdo,obj):

    if criterion == 'Any':
        acceptable_loudspeakers = loudspeakers
    else:
        acceptable_loudspeakers = [l for l in loudspeakers if l[criterion] == value]

    # If MDO is being forced, remove non-MDO loudspeakers from acceptable_loudspeakers
    if force_mdo:
        acceptable_loudspeakers = [l for l in acceptable_loudspeakers if l['mdo'] == 1]

    # Certain object types can only be in loudspeakers of a certain function
    if 'function' in obj:
        function = obj['function']
    else:
        function = 'any'

    if function=='narrator':
        acceptable_loudspeakers = [l for l in acceptable_loudspeakers if l['function'] == 'primary']
    elif function=='ambience':
        acceptable_loudspeakers = [l for l in acceptable_loudspeakers if l['function'] == 'secondary']

    # Loudspeaker switch - MDO loudspeakers can be turned on or off
    acceptable_loudspeakers = [l for l in acceptable_loudspeakers if l['switch'] == 1]

    return acceptable_loudspeakers


# FUNCTION: get object position
def Get_Object_Position(object,coordinate_system):

    #print('*** In Get_Object_Position ***')

    if 'direction' in object:
        # Spherical coordinates in object
        position_type = 'spherical'
        object_position = (object['direction']['az'],object['direction']['el'],object['direction']['refdist'])

    elif 'position' in object and 'az' in object['position']:
        position_type = 'spherical'
        object_position = (object['position']['az'],object['position']['el'],object['position']['refdist'])

    elif 'position' in object and 'x' in object['position']:
        # Cartesian coordinates in object
        position_type = 'cartesian'
        object_position = (object['position']['x'],object['position']['y'],object['position']['z'])

    else:
        # No position data (e.g. it's a diffuse object, HOA source, or channel object)
        position_type = 'cartesian'
        object_position = (0,0,0)  # TODO: At the moment this just returns at the origin


    # Convert the object position to the desired coordinate system
    if position_type == 'cartesian' and coordinate_system == 'spherical':
        az,el,refdist = cart2sphDeg(object_position[0],object_position[1],object_position[2])
        position = (az,el,refdist)

    elif position_type == 'spherical' and coordinate_system == 'cartesian':
        x,y,z = sphDeg2cart(object_position[0],object_position[1],object_position[2])
        position = (x,y,z)

    else:
        position = object_position

    # Convert object position to floats
    position = [float(p) for p in object_position]

    #print('*** Exiting Get_Object_Position ***')

    return position


def Print_ObjectVectorMessage(objectVector,loudspeakers):

    if loudspeakers != None:
        mdo_speakers = List_Acceptable_Loudspeakers(loudspeakers, 'Any', 'Any', 1, {})
    else:
        return

    print("Sent object vector with %i objects. %i loudspeakers available for MDO:" % (len(objectVector), len(mdo_speakers)))

    for speaker in mdo_speakers:

        object_string = ''

        for obj in objectVector:
            if obj['type'] != 'channel' or int(obj['outputChannels']) != int(speaker['id']):
                continue

            else:

                if 'content_label' in obj:
                    object_string = object_string + obj['content_label'] + ', '
                else:
                    #object_string = object_string + '(unlabelled object, ID: ' + str(obj['id']) + ', '
                    object_string = object_string + str(obj['id']) + ', '

        object_string = object_string[:-2]   # Remove the last ', '

        print("    Speaker %i (%s): %s" % (speaker['id'], speaker['name'], object_string))

        oscaddress = "/speaker%i" % int(speaker['id'])

        # Send OSC message to interface
        send_loudspeaker_content(object_string,oscaddress,'127.0.0.1',4555)



    print

    return


class MDOAzimuthProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)

        self.room_filename = 'None'
        self.loudspeaker_config = 'None'

        if 'on' in arguments.attrib:
            self.on = int(arguments.attrib['on'])
        else:
            self.on = 1 # Default is on

        if 'priority' in arguments.attrib:
            self.priority = arguments.attrib['priority']
        else:
            self.priority = 'quality' # Default is 'quality'

        if 'loudspeakerconfig' in arguments.attrib:
            self.loudspeaker_config = arguments.attrib['loudspeakerconfig']
        else:
            self.loudspeaker_config = 'None'

        if 'roomdescription' in arguments.attrib:
            self.room_filename = arguments.attrib['roomdescription']
        else:
            self.room_filename = 'None'

        if 'verbose' in arguments.attrib:
            self.verbose = arguments.attrib['verbose']
        else:
            self.verbose = 1

        #self.room_filename = arguments.attrib['roomdescription']
        #self.loudspeaker_config = arguments.attrib['loudspeakerconfig']


        #print "MDO status: %i.   Priority: %s" % (self.on, self.priority)

        # On initialisation, try parsing the files. But they'll both be none currently
        # TODO: These could be settable in the metadapter config file?
        self.loudspeakers = Parse_Loudspeaker_Config(self.loudspeaker_config)
        self.room_objects = Parse_Room_Description(self.room_filename)

        # Flag for using advanced metadata (rather than group and priority)
        self.use_amd = 1

        # Count the number of periods elapsed (this is used to print output less frequently)
        self.periods = 0

        #leveldB = float(arguments.attrib['volumeAdjust'])
        #self.volumeChange = numpy.power( 10.0, leveldB/20.0 )


    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.

        #Find_Nearest_Loudspeaker(self.room_objects,self.loudspeakers)

        if self.on:

            #print("MDO processor: length of object vector: %i" % len(objectVector))

            for i,obj in enumerate(objectVector):

                if self.use_amd:
                    if 'loudspeaker_quality' in obj:
                        current_quality = obj['loudspeaker_quality']
                    else:
                        current_quality = 'any' # If loudspeaker_quality isn't specified, it can be any (note: may want to make this 'high' for safety)

                    if 'room_object' in obj:
                        current_roomobject = obj['room_object']
                    else:
                        current_roomobject = 'any'  # If room_object isn't specified, it can be any (note: may want to make this 'none' for safety — none ensures that it's a VBAP object)

                    if 'force_mdo' in obj:
                        force_mdo = int(obj['force_mdo'])
                    else:
                        force_mdo = 0                # If not explicitly told to force MDO, don't

                else: # Don't use AMD, use group and priority flags
                    current_roomobject = obj['group']
                    current_quality = obj['priority']  # Quality flag
                    force_mdo = 0                       # Don't force MDO if not using advanced metadata (no way to specify)

                    # Convert numbers to strings
                    quality_strings = ['any','low','medium','high']
                    current_quality = quality_strings[current_quality]

                    roomobject_strings = ['none','bed','books','ceiling','chair','floor','furniture','objects','picture','sofa','table','tv','unknown','wall','window','any']
                    current_roomobject = roomobject_strings[current_roomobject]


                #print('    Loudspeaker quality: %s' % current_quality)
                #print('    Room object: %s' % current_roomobject)

                # Get object position
                current_position = Get_Object_Position(obj,'cartesian')
                #print('    Object position: %f %f %f' % (current_position[0], current_position[1], current_position[2]))


                if current_roomobject == 'none':
                    # Do nothing — it's a VBAP object
                    op_chan = 'None'

                # Special processing for speech group:
                #elif 'group_name' in obj and obj['group_name'] == 'speech':

                else: # Do MDO processing

                    if self.priority == "quality":

                        if current_quality == 'any': # Prioritise quality, quality isn't specified

                            if current_roomobject == 'any': # Any room object

                                # Get acceptable loudspeakers (any, but might have to force mdo)
                                acceptable_loudspeakers = List_Acceptable_Loudspeakers(self.loudspeakers,'Any','Any',force_mdo, obj)

                                # OUTPUT BRANCH 3: PUT IN THE LOUDSPEAKER NEAREST THE METADATA LOCATION
                                op_chan = Find_Nearest_Loudspeaker(current_position,acceptable_loudspeakers)

                            else: # Room object is specified

                                # Find the correct room objects
                                correct_roomobjects = [ro for i, ro in enumerate(self.room_objects) if str.lower(self.room_objects[i]['name']) == current_roomobject]

                                if len(correct_roomobjects) == 0: # There are none of the correct room objects

                                    # Get acceptable loudspeakers (any, but might have to force mdo)
                                    acceptable_loudspeakers = List_Acceptable_Loudspeakers(self.loudspeakers, 'Any','Any', force_mdo, obj)

                                    #OUTPUT BRANCH 3: PUT IN THE LOUDSPEAKER NEAREST THE METADATA LOCATION
                                    op_chan = Find_Nearest_Loudspeaker(current_position,acceptable_loudspeakers)

                                else: # The correct room object does exist...

                                    # TODO: For now, the first room object will be chosen. This should be adapted to look for the correct room object which has a loudspeaker (of acceptable quality) that's closest
                                    roomobject_coords = (correct_roomobjects[0]['x'],correct_roomobjects[0]['y'],correct_roomobjects[0]['z'])

                                    # Get acceptable loudspeakers (any, but might have to force mdo)
                                    acceptable_loudspeakers = List_Acceptable_Loudspeakers(self.loudspeakers, 'Any','Any', force_mdo, obj)

                                    # OUTPUT BRANCH 4: PUT IN LOUDSPEAKER NEAREST TO THE ROOM OBJECT
                                    op_chan = Find_Nearest_Loudspeaker(roomobject_coords,acceptable_loudspeakers)



                        else: # Prioritise quality, quality is specified

                            # Find the correct loudspeakers
                            acceptable_loudspeakers = List_Acceptable_Loudspeakers(self.loudspeakers,'quality',current_quality,force_mdo, obj)

                            if len(acceptable_loudspeakers) == 0: # There are none of the correct room objects
                                #CAN'T FIND A LOUDSPEAKER OF THE REQUESTED QUALITY, SO PUTTING BACK IN VBAP
                                # TODO: this could be changed in due course to allow it to go near a reasonable substitute, but for the moment it's safer to go back to VBAP
                                op_chan = 'None'

                            else:
                                # CAN FIND A LOUDSPEAKER OF ACCEPTABLE QUALITY

                                if current_roomobject == 'any': # Any loudspeaker of acceptable quality

                                    # OUTPUT BRANCH 1: PUT IN LOUDSPEAKER OF ACCEPTABLE QUALITY NEAREST THE METADATA LOCATION
                                    op_chan = Find_Nearest_Loudspeaker(current_position,acceptable_loudspeakers)


                                else: # There's a room object as well as a loudspeaker quality

                                    # Find the correct room objects
                                    correct_roomobjects = [ro for i,ro in enumerate(self.room_objects) if str.lower(self.room_objects[i]['name'])==current_roomobject]

                                    if len(correct_roomobjects) == 0: # There are none of the correct room objects

                                        # OUTPUT BRANCH 1: PUT IN LOUDSPEAKER OF ACCEPTABLE QUALITY NEAREST THE METADATA LOCATION
                                        op_chan = Find_Nearest_Loudspeaker(current_position,acceptable_loudspeakers)

                                    else: # The correct room object does exist...

                                        # TODO: For now, the first room object will be chosen. This should be adapted to look for the correct room object which has a loudspeaker (of acceptable quality) that's closest
                                        roomobject_coords = (correct_roomobjects[0]['x'],correct_roomobjects[0]['y'],correct_roomobjects[0]['z'])

                                        # OUTPUT BRANCH 2: PUT IN LOUDSPEAKER OF ACCEPTABLE QUALITY NEAREST TO THE ROOM OBJECT
                                        op_chan = Find_Nearest_Loudspeaker(roomobject_coords,acceptable_loudspeakers)




                    elif self.priority == 'roomobject':

                        if current_roomobject == 'any':

                            if current_quality == 'any': # No object and no quality specified

                                # Get acceptable loudspeakers (any, but might have to force mdo)
                                acceptable_loudspeakers = List_Acceptable_Loudspeakers(self.loudspeakers, 'Any', 'Any',force_mdo, obj)

                                #OUTPUT BRANCH 3: PUT IN THE LOUDSPEAKER NEAREST THE METADATA LOCATION
                                op_chan = Find_Nearest_Loudspeaker(current_position,acceptable_loudspeakers)

                            else: # Quality is specified

                                # Get acceptable loudspeakers (any, but might have to force mdo)
                                acceptable_loudspeakers = List_Acceptable_Loudspeakers(self.loudspeakers, 'quality',current_quality,force_mdo, obj)

                                if len(acceptable_loudspeakers) == 0: # There are none of the correct loudspeakers
                                    # BECAUSE IT'S ROOM OBJECT PRIORITY, THIS WILL BE RETAINED
                                    # TODO: could add a 'safe mode' flag to this so that loudspeaker quality is never compromised, or search for a similar quality loudspeaker

                                    # Get acceptable loudspeakers (any, but might have to force mdo)
                                    acceptable_loudspeakers = List_Acceptable_Loudspeakers(self.loudspeakers, 'Any','Any', force_mdo, obj)

                                    #OUTPUT BRANCH 3: PUT IN THE LOUDSPEAKER NEAREST THE METADATA LOCATION
                                    op_chan = Find_Nearest_Loudspeaker(current_position,acceptable_loudspeakers)

                                else: # An acceptable loudspeaker does exist

                                     # OUTPUT BRANCH 1: PUT IN LOUDSPEAKER OF ACCEPTABLE QUALITY NEAREST THE METADATA LOCATION
                                    op_chan = Find_Nearest_Loudspeaker(current_position,acceptable_loudspeakers)

                        else: # current_roomobject is specified

                            # See if the room object exists
                            correct_roomobjects = [ro for i, ro in enumerate(self.room_objects) if str.lower(self.room_objects[i]['name']) == current_roomobject]

                            if len(correct_roomobjects) == 0: # There are none of the correct room objects

                                if current_quality == 'any': # No object and no quality specified

                                    # Get acceptable loudspeakers (any, but might have to force mdo)
                                    acceptable_loudspeakers = List_Acceptable_Loudspeakers(self.loudspeakers, 'Any','Any', force_mdo, obj)

                                    #OUTPUT BRANCH 3: PUT IN THE LOUDSPEAKER NEAREST THE METADATA LOCATION
                                    op_chan = Find_Nearest_Loudspeaker(current_position,acceptable_loudspeakers)

                                else: # Quality is specified

                                    # Get acceptable loudspeakers (any, but might have to force mdo)
                                    acceptable_loudspeakers = List_Acceptable_Loudspeakers(self.loudspeakers, 'quality',current_quality,force_mdo, obj)

                                    if len(acceptable_loudspeakers) == 0: # There are none of the correct loudspeakers
                                        # BECAUSE IT'S ROOM OBJECT PRIORITY, THIS WILL BE RETAINED
                                        # TODO: could add a 'safe mode' flag to this so that loudspeaker quality is never compromised, or search for a similar quality loudspeaker

                                        # Get acceptable loudspeakers (any, but might have to force mdo)
                                        acceptable_loudspeakers = List_Acceptable_Loudspeakers(self.loudspeakers,'Any','Any',force_mdo, obj)

                                        #OUTPUT BRANCH 3: PUT IN THE LOUDSPEAKER NEAREST THE METADATA LOCATION
                                        op_chan = Find_Nearest_Loudspeaker(current_position,acceptable_loudspeakers)

                                    else: # An acceptable loudspeaker does exist

                                         # OUTPUT BRANCH 1: PUT IN LOUDSPEAKER OF ACCEPTABLE QUALITY NEAREST THE METADATA LOCATION
                                        op_chan = Find_Nearest_Loudspeaker(current_position,acceptable_loudspeakers)



                            else: # The correct room object does exist...

                                # TODO: For now, the first room object will be chosen. This should be adapted to look for the correct room object which has a loudspeaker (of acceptable quality) that's closest
                                roomobject_coords = (correct_roomobjects[0]['x'],correct_roomobjects[0]['y'],correct_roomobjects[0]['z'])

                                # Check for quality
                                if current_quality == 'any':

                                    # Get acceptable loudspeakers (any, but might have to force mdo)
                                    acceptable_loudspeakers = List_Acceptable_Loudspeakers(self.loudspeakers,'Any','Any',force_mdo, obj)

                                    # OUTPUT BRANCH 4: PUT IN LOUDSPEAKER NEAREST TO THE ROOM OBJECT
                                    op_chan = Find_Nearest_Loudspeaker(roomobject_coords,acceptable_loudspeakers)

                                else: # There's a quality setting as well...

                                    # Get acceptable loudspeakers (any, but might have to force mdo)
                                    acceptable_loudspeakers = List_Acceptable_Loudspeakers(self.loudspeakers, 'quality',current_quality,force_mdo, obj)

                                    if len(acceptable_loudspeakers) == 0: # There are none of the correct loudspeakers
                                        # BECAUSE IT'S ROOM OBJECT PRIORITY, THIS WILL BE RETAINED
                                        # TODO: could add a 'safe mode' flag to this so that loudspeaker quality is never compromised, or search for a similar quality loudspeaker

                                        # Get acceptable loudspeakers (any, but might have to force mdo)
                                        acceptable_loudspeakers = List_Acceptable_Loudspeakers(self.loudspeakers, 'Any','Any', force_mdo, obj)

                                        # OUTPUT BRANCH 4: PUT IN LOUDSPEAKER NEAREST TO THE ROOM OBJECT
                                        op_chan = Find_Nearest_Loudspeaker(roomobject_coords,acceptable_loudspeakers)

                                    else: # An acceptable loudspeaker does exist

                                        # OUTPUT BRANCH 2: PUT IN LOUDSPEAKER OF ACCEPTABLE QUALITY NEAREST TO THE ROOM OBJECT
                                        op_chan = Find_Nearest_Loudspeaker(roomobject_coords,acceptable_loudspeakers)

                    elif self.priority == 'location':

                        # At the moment, location just puts into the nearest MDO loudspeaker, ignoring other settings.
                        # TODO: Could add other checks (e.g. closest correct loudspeaker)

                        acceptable_loudspeakers = List_Acceptable_Loudspeakers(self.loudspeakers, 'Any', 'Any', force_mdo, obj)

                        if len(acceptable_loudspeakers) == 0:
                            op_chan = 'None'

                        #if int(obj['id']) == 0:
                            #print acceptable_loudspeakers
                            #print("Object 1 position: %s" % current_position)

                        # OUTPUT BRANCH 2: PUT IN LOUDSPEAKER OF ACCEPTABLE QUALITY NEAREST TO THE ROOM OBJECT
                        op_chan = Find_Nearest_Loudspeaker(current_position, acceptable_loudspeakers)


                    else: # self.priority = ???
                        print( "Priority setting not recognised. Keeping VBAP" )
                        op_chan = 'None'



                if op_chan == "None":

                    if self.verbose and False: # Turned off this print
                        print( "Object %i.    Force MDO: %s.    No channel object" % (int(obj['id']), ['no','yes'][force_mdo]) )

                else:

                    # If it's been decided from the rules above that the object should be put into a VBAP loudspeaker, then don't make it a channel object
                    # ToDo: This might be decided in a more intelligent way, or based on the priority settings

                    # Find the loudspeaker that was selected
                    for l in self.loudspeakers:
                        if int(l['id']) == int(op_chan):
                            if int(l['mdo']) == 1:
                                # Do the object change here. op_chan is the new channel
                                obj['type'] = 'channel'
                                obj['outputChannels'] = op_chan
                            #else: # it's not MDO, so there's no change


                    if self.verbose and False: # Turned off this print
                        print( "Object %i.    Force MDO: %s.    New channel: %i" % (int(obj['id']), ['no','yes'][force_mdo], op_chan))



            # Each period, increment the count
            self.periods += 1

            if self.periods > 49:
                self.periods = 1
                if self.verbose:
                    Print_ObjectVectorMessage(objectVector,self.loudspeakers)


        # Note: this has to be returned otherwise no objects are replayed
        #print objectVector
        return objectVector



    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""

        if key == "on":
            if valueList[0] == 0 or valueList[0] == 1:
                self.on = valueList[0]
                print('MDO processor status: %s' % ['off','on'][self.on])
            else:
                raise KeyError( "Command \"on\" must be 0 or 1" )

            if self.on:
                self.periods = 50

            # When an off message is recieved, send empty strings to each of the MDO loudspeaker content descriptions
            if self.on == 0:
                for l in self.loudspeakers:
                    if 'mdo' in l and int(l['mdo']) == 1:
                        oscaddress = "/speaker%i" % int(l['id'])
                        send_loudspeaker_content("",oscaddress,'127.0.0.1',4555)


        elif key == "verbose":
            if valueList[0] == 0 or valueList[0] == 1:
                self.verbose = valueList[0]
            else:
                raise KeyError( "Command \"verbose\" must be 0 or 1" )

        elif key == "priority":

            self.periods = 50

            self.priority = valueList[0]
            if self.verbose:
                print("MDO processor priority set to: %s" % self.priority)

        elif key == "roomdescription":
            self.room_filename = valueList[0]
            self.room_objects = Parse_Room_Description(self.room_filename)

            self.periods = 50

        elif key == "loudspeakerconfig":
            self.loudspeaker_config = valueList[0]
            self.loudspeakers = Parse_Loudspeaker_Config(self.loudspeaker_config)

            self.periods = 50

        elif key == "loudspeakerswitch":      # Can turn individual MDO loudspeakers on and off
            self.periods = 50

            switch_id = int(valueList[0])
            switch_status = int(valueList[1])

            for l in self.loudspeakers:                 # Look through the speakers
                if int(l['id']) == switch_id:           # If it's the matching ID
                    l['switch'] = switch_status         # Set it's status

                    if switch_status == 0:
                        oscaddress = "/speaker%i" % switch_id
                        send_loudspeaker_content("", oscaddress, '127.0.0.1', 4555)

        #elif key == "reset":    # Reset the strings containing object names





        else:
            raise KeyError( "MDOProcessor supports only the parameter set commands \"on\", \"verbose\", \"priority\", \"roomdescription\", \"loudspeakerconfig\", or \"loudspeakerswitch\"")



        #if (key != "envelopment") or (len( valueList ) != 1) or (not isinstance(valueList[0], float) ):
        #    raise KeyError( "MDOProcessor supports only the parameter set command \"envelopment\" consisting of a single float value" )
        #self.env = valueList[0]
        #print "Envelopment set at %2.2f" % (self.env)
