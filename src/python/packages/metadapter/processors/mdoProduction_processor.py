# -*- coding: utf-8 -*-
"""
Created on Fri Feb 03 2017

@author: Jon Francombe
"""

import numpy
import math
import json
import random
import copy
from time import gmtime

from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor

#from sys import version_info
#
#if version_info.major <= 2:
#    import OSC
#else:
#    # Use the self-made port for Python 3 (experimental)
#    import thirdparty.OSC as OSC

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
                name = 'no name'    # Unknown name is set as 'no name'

            if 'mdo' in child.attrib:           # If the MDO attribute exists
                mdo = int(child.attrib['mdo'])  # ...see what it is
            else:                               # Otherwise, it doesn't exist
                mdo = 0                         # ...set MDO to zero

            if 'quality' in child.attrib:
                quality = child.attrib['quality']
            else:
                quality = 'unknown'     # Unknown quality is set as unknown

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
				
            if 'mdoZone' in child.attrib:
                mdoZone = int(child.attrib['mdoZone'])
            else:
                mdoZone = -1

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
            loudspeakers.append( dict(name=name, id=id, channel=channel, mdo=mdo, quality=quality, type=type, x=x, y=y, z=z, switch=switch, function=function, mdoZone=mdoZone) )
    
    return loudspeakers


## FUNCTION: find nearest loudspeaker (return the channel number)
#def Find_Nearest_Loudspeaker(point,loudspeakers):
#    norm_dist = True
#    if norm_dist == True:
#        #print point
#        mag = math.sqrt(point[0]**2 + point[1]**2 + point[2]**2)
#        point = [n/mag for n in point]
#        #print point
#        
#    if type(point) is list:
#        # Convert list to tuple
#        point = tuple(point)    
#    
#
#    from scipy.spatial import distance
#
#    dist = []
#    az_diff = []
#    ls_az_list = []
#
#    for l in loudspeakers:
#
#        # Get the loudspeaker position
#        if norm_dist == True:
#            l_norm = [l['x'], l['y'], l['z']]
#            mag = math.sqrt(l_norm[0]**2 + l_norm[1]**2 + l_norm[2]**2)
#            l_norm = [n/mag for n in l_norm]
#            ls_coords = (l_norm[0], l_norm[1], l_norm[2])
#        else:        
#            ls_coords = (l['x'], l['y'], l['z'])
#        
#        # Get speaker azimuths
#        ls_az = math.atan2(l['y'],l['x'])%(2*math.pi)
#        ls_az_list.append(ls_az*180/math.pi)
#        #print ls_az*180/math.pi
#        
#        # Get point azimuth
#        point_az = math.atan2(point[1],point[0])%(2*math.pi)
#        
#        # Calculate the euclidean distance
#        dist.append(distance.euclidean(point,ls_coords))
#        
#        # Calculate angle differences
#        az_diff.append(abs(ls_az - point_az))
#        
#        # Print
#        #print("Loudspeaker %i (%s) position: %s" %(int(l['id']),l['name'],ls_coords))
#
#
#    if len(dist) == 0:
#        closest_channel = 'None'
#
#    else:
#
#        # Find the loudspeaker with the smallest euclidean distance
#        closest = dist.index(min(dist)) # For now, just get the first minimum distance
#        
#        closest_az = az_diff.index(min(az_diff))
#        #print "Target azimuth:"
#        #print point_az*180/math.pi
#        
#        #print "Closest LS azimuth:"
#        #print ls_az_list[closest_az]
#
#        # Get the ID from the closest loudspeaker
#        closest_channel = loudspeakers[closest]['id']
#
#    # Return the closest channel number
#    return closest_channel


## FUNCTION: list acceptable loudspeakers
#def List_Acceptable_Loudspeakers(loudspeakers,criterion,value,force_mdo,obj):
#
#    if criterion == 'Any':
#        acceptable_loudspeakers = loudspeakers
#    else:
#        acceptable_loudspeakers = [l for l in loudspeakers if l[criterion] == value]
#
#    # If MDO is being forced, remove non-MDO loudspeakers from acceptable_loudspeakers
#    if force_mdo:
#        acceptable_loudspeakers = [l for l in acceptable_loudspeakers if l['mdo'] == 1]
#
#    # Certain object types can only be in loudspeakers of a certain function
#    if 'function' in obj:
#        function = obj['function']
#    else:
#        function = 'any'
#
#    if function=='narrator':
#        acceptable_loudspeakers = [l for l in acceptable_loudspeakers if l['function'] == 'primary']
#    elif function=='ambience':
#        acceptable_loudspeakers = [l for l in acceptable_loudspeakers if l['function'] == 'secondary']
#
#    # Loudspeaker switch - MDO loudspeakers can be turned on or off
#    acceptable_loudspeakers = [l for l in acceptable_loudspeakers if l['switch'] == 1]
#
#    return acceptable_loudspeakers


# FUNCTION: print what's in loudspeakers data
def print_loudspeakers(loudspeakers):
    
    print("\nFound %i loudspeakers:" % len(loudspeakers))
    
    # Get keys
    lsKeys = []
    for ls in loudspeakers:
        for k in ls:
            if not k in lsKeys:
                lsKeys.append(k)
    
    # Loop round speakers and print details
    for i, ls in enumerate(loudspeakers):
        string = '%7i.' % i
        for k in lsKeys:
            if k in ls:
                value = ls[k]
                if isinstance(value,int):
                    value = '%3i' % value
                elif isinstance(value,float):
                    value = '%5.2f' % value
                else:
                    value = '%7.7s' % str(value)
            else:
                value = 'Unknown'
            string = string+(' %s: %s ; ' % (k,value))
        print(string)
    print('')

    return


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


#def Print_ObjectVectorMessage(objectVector,loudspeakers):
#
#    if loudspeakers != None:
#        mdo_speakers = List_Acceptable_Loudspeakers(loudspeakers, 'Any', 'Any', 1, {})
#    else:
#        return
#
#    print("Sent object vector with %i objects. %i loudspeakers available for MDO:" % (len(objectVector), len(mdo_speakers)))
#
#    for speaker in mdo_speakers:
#
#        object_string = ''
#
#        for obj in objectVector:
#            if obj['type'] != 'channel' or int(obj['outputChannels']) != int(speaker['id']):
#                continue
#
#            else:
#
#                if 'content_label' in obj:
#                    object_string = object_string + obj['content_label'] + ', '
#                else:
#                    #object_string = object_string + '(unlabelled object, ID: ' + str(obj['id']) + ', '
#                    object_string = object_string + str(obj['id']) + ', '
#
#        object_string = object_string[:-2]   # Remove the last ', '
#
#        print("    Speaker %i (%s): %s" % (speaker['id'], speaker['name'], object_string))
#
#    print
#
#    return
    


def get_mdo_speakers(self):
    
    ''' Function to get active MDO devices (and 'zones') '''
    
    # If default is to assume MDO device or not
    isMdoDefault = False
    
    # Get available MDO speaker ids and zones (in order they joined)
    MDOactive = []
    MDOorder = []
    MDOzone = []
    for i,ls in enumerate(self.loudspeakers):
        # Check if MDO
        if 'mdo' in ls:
            isMdo = bool(int(ls['mdo']))
        else:
            isMdo = isMdoDefault
        # Check if on
        if 'switch' in ls:
            LSon = bool(int(ls['switch']))
        else:
            LSon = True     # Default assume is on
        if isMdo:                              # If MDO device...
            if LSon:                               # And if on...
                # Add id to MDO list
                MDOactive.append(int(ls['id']))
                # Add order joined (-1 flag if this is not known)
                if 'joined' in ls:
                    join_value = int(ls['joined'])
                    if join_value < 1:
                        join_value = -1
                else:
                    join_value = -1
                MDOorder.append(join_value)
                # Add zone of device (-1 flag if this is not known)
                if 'mdoZone' in ls:
                    zone_value = int(ls['mdoZone'])
                    if zone_value < 1:
                        zone_value = -1
                else:
                    zone_value = -1
                MDOzone.append(zone_value)
    
    MDOactive = numpy.array(MDOactive)
    MDOorder = numpy.array(MDOorder)
    MDOzone = numpy.array(MDOzone)
        
    # Number of MDO devices available
    NMDO = len(MDOactive)
    
    # Need to list active devices in order, putting any unknowns (-1s) to end
    if NMDO > 0:
        MDOorder[MDOorder==-1] = numpy.max(MDOorder)+1
        MDOactive = MDOactive[numpy.argsort(MDOorder)]
        MDOzone = MDOzone[numpy.argsort(MDOorder)]
    
    if self.verbose:
        print('\nMDO speaker ids available:',MDOactive,'in zones:',MDOzone,'\n')
        
    return MDOactive, MDOzone, NMDO



def zone_method(MDOactive,MDOzone,NMDO,OBJzones,OBJprevious,OBJprevzone,OBJdynamic,anyPrevious,isPrevious,objName,verbose):
    ''' Function to calculate MDO routing based on zonal method'''
    
    # Maximum zone priority
    zone_priority = numpy.max(OBJzones)
    
    # If the object is static and has previous routing...
    if not OBJdynamic and isPrevious:
        # Zone result must be better than previous in order to change
        zone_min = max(OBJprevzone+1,1)
        # Assign previous in case don't improve
        op_id = OBJprevious
        op_zone = int(MDOzone[anyPrevious[0]])        
        op_zone_level = OBJprevzone
    else:
        # Zone result calculated afresh
        zone_min = 1
        # Assign defaults in case don't improve
        op_id = -1
        op_zone = -1
        op_zone_level = -1
    if verbose:
        print('%s - Looking for MDO speaker in min zone level: %i' % (objName,zone_min))
        
    # Check zone priorities in decreasing order until found device (or no zones left)
    found_new_zone = False
    while zone_priority >= zone_min:
        # Get zones in which object set to current zone priority
        in_zone = [j+1 for j,x in enumerate(OBJzones) if x==zone_priority]
        Nzone = len(in_zone)
        # Check each of the zones at this priority in a random order
        rand_order = random.sample(range(0,Nzone),Nzone)
        izone = 0
        while izone < Nzone:
            check_zone = in_zone[rand_order[izone]]
            # Check each of the devices in order (the order they joined)
            iMDO = 0
            if not OBJdynamic:
                # For static, check devices in order they joined
                ind = range(0,NMDO)
            else:
                # For dynamic, check devices in random order
                ind = random.sample(range(0,NMDO),NMDO)
            while iMDO < NMDO:
                indi = ind[iMDO]
                if MDOzone[indi]==check_zone:
                    # If found, use this output id
                    op_id = MDOactive[indi]
                    op_zone = MDOzone[indi]
                    op_zone_level = zone_priority
                    # To end while loops...
                    zone_priority = 0
                    izone = Nzone
                    iMDO = NMDO
                    found_new_zone = True
                iMDO = iMDO+1
            izone = izone+1
        zone_priority = zone_priority-1     # Decrement zone to look at
    if verbose:
        print(objName,'- New zone routing found:',found_new_zone)
    
    # If exits above while loops withut finding device, op_id should remain unchanged
    
    return op_id, op_zone, op_zone_level



def nearest_neighbour_method(obj,loudspeakers,MDOactive,i,verbose):
    
    ''' Function to calculate MDO routing based on nearest neighbour method'''
    
    # Whether to do normalised version
    norm_dist = False
    
    if any(MDOactive):
        
        # Get position of object in cartesian coordinates
        position = Get_Object_Position(obj,'cartesian')
        
        # If normalise object position vector
        if norm_dist:
            mag = math.sqrt(position[0]**2 + position[1]**2 + position[2]**2)
            position = numpy.divide(position,mag)
        
        if type(position) is list:
            # Convert list to tuple
            position = tuple(position)
            
        # Get loudspeaker coordinates as floats
        xx = []
        yy = []
        zz = []
        for x in loudspeakers:
            # If an active MDO speaker
            if any(int(x['id'])==MDOactive):
                xi = float(x['x'])
                yi = float(x['y'])
                zi = float(x['z'])
                if norm_dist:
                    mag = math.sqrt(xi**2+yi**2+zi**2)
                    if mag == 0:
                        xi = 0.0
                        yi = 0.0
                        zi = 0.0
                    else:
                        xi = xi/mag
                        yi = yi/mag
                        zi = zi/mag
                xx.append(xi)
                yy.append(yi)
                zz.append(zi)
        
        # Get (square of) distances between
        dist = []
        for i in range(0,len(xx)):
            LSxyz = (xx[i],yy[i],zz[i])
            dist.append(numpy.sum(numpy.subtract(position,LSxyz)**2))
        
        # Find_Nearest_Loudspeaker(position,loudspeakers)
        closest = dist.index(min(dist))
        
        # Loudspeaker id
        op_id = MDOactive[closest]
    
    else:
        
        # Default if don't find anything
        op_id = -1
    
    return op_id


def update_loudspeaker_data(self,verboseCheck=True):
    
    ''' Function to update current loudspeaker data'''
    
    # Update speakers if LSupdate flag is true
    if self.LSupdate == True:
        # Reset loudspeaker settings to those in config file
        self.loudspeakers = copy.deepcopy(self.loudspeaker_config)
        # Verbose printout
        if self.verbose0:
            #print('\nReceived loudspeaker update:\n',self.loudspeakers)
            print('\nReceived loudspeaker update\n')
        # Update loudspeaker values
        for ls in self.loudspeakers:
            lsId = int(ls["id"])
            if lsId in self.lsData:
                ls.update( self.lsData[lsId] )
    
    # Make in correct format
    for ls in self.loudspeakers:
        
        if 'id' in ls:
            ls['id'] = int(ls['id'])
        
        if 'name' in ls:
            ls['label'] = str(ls['name'])
        elif 'label' in ls:
            ls['label'] = str(ls['label'])
        
        if 'channel' in ls:
            ls['channel'] = int(ls['channel'])
            if verboseCheck:
                print('\nLoudspeaker %i: channel updates will have no effect\n' % ls['id'])
        
        if 'mdo' in ls:
            ls['mdo'] = bool(int(ls['mdo']))
            if verboseCheck and not ls['mdo']:
                print('\nLoudspeaker %i: updates to non-MDO loudspeakers may have no effect\n' % ls['id'])
        
        if 'x' in ls:
            ls['x'] = float(ls['x'])
        if 'y' in ls:
            ls['y'] = float(ls['y'])
        if 'z' in ls:
            ls['z'] = float(ls['z'])
        
        if 'mdoZone' in ls:
            ls['mdoZone'] = int(ls['mdoZone'])
        if 'switch' in ls:
            ls['switch'] = bool(int(ls['switch']))
        if 'joined' in ls:
            ls['joined'] = int(ls['joined'])
        if 'mdoGainDB' in ls:
            ls['mdoGainDB'] = float(ls['mdoGainDB'])
    
    # Print loudspeaker list
    if self.verbose0:
        print_loudspeakers(self.loudspeakers)
            
    return self

def getLsData(self,allData):
    """ Internal function to get object data from dict """
    # Extract the actual table.
    # Depends on the actualsend format.
    if 'mdoLoudspeakers' in allData:
        tableData = allData['mdoLoudspeakers']
        # Store the received data in a lookup table (a dict) with the id as search index.
        # Note: Revisit whether the ID should be transmitted as "ID"
        self.lsData = {}
        for ls in tableData:
            #objId = int(obj.pop( "ID" ))
            if 'id' in ls:
                lsId = int(ls.pop( "id" )) # Loudspeakers ids are not zero indexed
            elif 'speakerNumber' in ls:
                lsId = int(ls.pop( "speakerNumber" ))
            self.lsData[lsId] = ls
        # Unfortunately this does not work, because id is removed before it can be used for the index
        # int(obj["ID"]): obj.pop("ID") for obj in tableData }
        self.LSupdate = True
        
    return self


class TableReceiver( DatagramProtocol ):
    """ Internal class to handle receipt of table messages """
    def __init__( self, parent, verbose=False ):
        super(TableReceiver).__init__()
        self.parent = parent
        self.verbose = verbose

    def startProtocol( self ):
        if self.verbose:
            print( 'NetworkUdpReceiver.startProtocol()' )
            # Nothing really to do here (for a receiver)
    def datagramReceived( self, data, address ):
        self.parent.datagramReceived( data, address )
        
class MDOProductionProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)
        
        # Get config inputs and defaults
        
        # If processor is on
        if 'on' in arguments.attrib:
            self.on = bool(int(arguments.attrib['on']))
        else:
            self.on = True # Default is on
           
        # If config file for loudspeakers
        if 'loudspeakerconfig' in arguments.attrib:
            #print(arguments.attrib['loudspeakerconfig'])
            self.loudspeaker_config = arguments.attrib['loudspeakerconfig']
        else:
            self.loudspeaker_config = 'None'
        
        # If verbose
        if 'verbose' in arguments.attrib:
            self.verbose = bool(int(arguments.attrib['verbose']))
        else:
            self.verbose = False
        self.verbose0 = self.verbose
		
        # If message packet size specified
        if "messagePacketSize" in arguments.attrib:
            inputPacketSize =  int(arguments.attrib[ "messagePacketSize" ])
        else:
            inputPacketSize = None
        
        # If UDP port for receiving updated loudspeaker settings on
        if "udpPort" in arguments.attrib:
            self.udpPort = int(arguments.attrib[ "udpPort" ])
        else:
            self.udpPort = 5000
        
        # Rendering method to use
        if "method" in arguments.attrib:
            self.method = int(arguments.attrib[ "method" ])
        else:
            self.method = 3
        
        # Number of zones to use (for certain rendering methods)
        if "numberZones" in arguments.attrib:
            self.MaxZones = int(arguments.attrib[ "numberZones" ])
        else:
            self.MaxZones = 4
        
        # Force update of devices
        if "updateDevices" in arguments.attrib:
            self.LSupdate = bool(int(arguments.attrib[ "updateDevices" ]))
        else:
            self.LSupdate = False
        
        # Refresh period for verbose messages
        if "verbosePeriod" in arguments.attrib:
            self.refreshPeriod = int(arguments.attrib[ "verbosePeriod" ])
        else:
            self.refreshPeriod = 50
        
        # Write received loudspeaker data to file if passed filepath prefix
        if "writeLsToFile" in arguments.attrib:
            self.writeLsToFile =  arguments.attrib[ "writeLsToFile" ]
        else:
            self.writeLsToFile = ''
        
        # Flag to clear any previous device history from objects
        self.LSclear = True
        
        # Try parsing loudspeaker config file
        self.loudspeaker_config = Parse_Loudspeaker_Config(self.loudspeaker_config) # Keep original copy
        self.loudspeakers = copy.deepcopy(self.loudspeaker_config)                  # Copy for using and updating
        self = update_loudspeaker_data(self,False)                                  # Because converts some data types and prints (without checking for stuff can't update)
        
        # Set up udp receive for loudspeaker data updates
        self.udpPort = self.udpPort
        self.udpReceiver = TableReceiver( self, verbose = True )
        if inputPacketSize == None:
            reactor.listenUDP( self.udpPort, self.udpReceiver )
        else:
            reactor.listenUDP( self.udpPort, self.udpReceiver, maxPacketSize=inputPacketSize )
        self.lsData = {}
        
        # Count the number of periods elapsed (this is used to print output less frequently)
        self.periods = 1
        
        # For keeping track of object history
        self.objHistory = {}

    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.
        
        # Because when been an update, converts some data types and then prints
        if self.LSupdate:
            self = update_loudspeaker_data(self)
        
        """ Do MDO processing """

        if self.on:     # Do stuff if processor turned on
            
            # Get available MDO speaker ids and zones (in order they joined)
            MDOactive,MDOzone,NMDO = get_mdo_speakers(self)
            
            for i,obj in enumerate(objectVector):
                #if int(obj["id"]) == 0:
                #    print(obj)
                # Loop round objects...
                
                # Get current id
                if 'id' in obj:
                    id_i = int(obj['id'])
                else:
                    id_i = -1
                    print('\nObject error - invalid id !!!\n')
                
                # Get label to print as (default based on object id, but 1 indexed)
                if 'label' in obj and len(obj['label']) > 0:
                    objName = str(obj['label'])
                else:
                    objName = 'Object %3i' % (id_i+1)
                objName = '%16.16s' % objName # Make fixed length (padded or truncated)
                
                # Get object history (if there is any)
                if id in self.objHistory:
                    objHistory = self.objHistory[id]
                else:
                    objHistory = {}
                
                # Clear device routing history from objects if requested
                if self.LSclear:
                    objHistory['mdoPreviousId'] = -1
                    objHistory['mdoPreviousZoneLevel'] = -1
#                    if 'mdoPreviousId' in obj:
#                        obj['mdoPreviousId'] = -1
#                    if 'mdoPreviousZoneLevel' in obj:
#                        obj['mdoPreviousZoneLevel'] = -1
                
                # Determine if object is active (and store in object history)
                if 'active' in obj:
                    objHistory['active'] = bool(obj['active'])
                else:
                    # Currently assume object is active unless find otherwise
                    objHistory['active'] = True
                
                # Check to see if any mute if other object active id passed
                if 'muteIfObject' in obj:
                    muteIfObject = int(obj['muteIfObject'])-1 # Offset by -1 because of zero indexing here
                else:
                    muteIfObject = -1 # Default no muting
                
                # Check if object can be MDO (at this point this is a "threshold")
                if 'mdoThreshold' in obj:
                    OBJismdo = int(obj['mdoThreshold'])
                elif 'ForceAuxiliary' in obj:
                    OBJismdo = int(obj['ForceAuxiliary']) # Legacy name
                else:
                    OBJismdo = 0 # Default not MDO
                
                # Check if MDO 'threshold' - only played if this many or more MDO devices
                if 'mdoOnlyThreshold' in obj:
                    mdoOnlyThreshold = int(obj['mdoOnlyThreshold'])
                else:
                    mdoOnlyThreshold = 0 # Default no threshold
                
                # Threshold for ONLY appearing if (minimum number of) MDO devices present
                if mdoOnlyThreshold > 0 and mdoOnlyThreshold > NMDO:
                    mdoOnlyThreshold = 0
                else:
                    mdoOnlyThreshold = 1
                
                # Chek if can be MDO
                if OBJismdo and not mdoOnlyThreshold:
                    # If object is MDO and MDO ONLY threshold is less than number of available devices
                    OBJismdo = 0 # Saves looking for MDO routing
                    objHistory['active'] = False
                    if self.verbose:
                        print('%s - Not enough MDO devices to unlock - object inactive' % objName)
                elif NMDO < 1:
                    # Not MDO if no devices
                     OBJismdo == 0 # Saves looking for MDO routing
                     if self.verbose:
                         print('%s - No MDO devices' % objName)
                elif NMDO < OBJismdo:
                    # If number of devices is less than "mdo threshold" it can't be mdo
                    OBJismdo = 0 # Saves looking for MDO routing
                    if self.verbose:
                        if OBJismdo:
                            print('%s - Not enough MDO devices to make MDO' % objName)
                else:
                    # Otherwise convert back to on/off flag
                    OBJismdo = numpy.minimum(OBJismdo,1)
                
                # Assume inactive if no level
                if float(obj['level']) <= 0.00001: # i.e. less than or equal to -100 dB
                    objHistory['active'] = False
                
                # Check if should be muted if other object active
                if muteIfObject in self.objHistory:
                    check_obj = self.objHistory[muteIfObject]
                    if check_obj['active']:
                        # Deactivate if check object is active
                        objHistory['active'] = False
                
                # Mute object if not active
                if not objHistory['active']:
                    # Mute object
                    obj['level'] = 0.0
                    OBJismdo == 0 # Saves looking for MDO routing
                
                if not OBJismdo:
                    
                    # Object can't be MDO...
                    
                    # Strip out any old mdoPrevious data and then do nothing
                    objHistory['mdoPreviousId'] = -1
                    objHistory['mdoPreviousZoneLevel'] = -1
#                    if 'mdoPreviousId' in obj:
#                        obj['mdoPreviousId'] = -1
#                    if 'mdoPreviousZoneLevel' in obj:
#                        obj['mdoPreviousZoneLevel'] = -1
                    if self.verbose:
                        print('%s - Not MDO' % (objName))
                    
                else:
                    
                    # Object could be MDO (so check)...
                    
                    # Check if object has specified rendering method
                    if 'mdoMethod' in obj:
                        OBJmethod = int(obj['mdoMethod'])
                        if OBJmethod < 0:
                            # Use default if integer negative
                            OBJmethod = self.method
                        elif OBJmethod > self.method:
                            # Use default if method 'better' than allowed
                            OBJmethod = self.method
                    else:
                        OBJmethod = self.method # Use default
                    method_print = OBJmethod
                    
                    # If method is 1, just use method 2 but with zones in the order the devices joined
                    if OBJmethod == 1:
                        OBJmethod = 2
                        # MDOzone = (numpy.array(range(0,NMDO))%self.MaxZones)+1 # Done modulo
                        MDOzone = numpy.minimum(numpy.array(range(0,NMDO))+1,self.MaxZones) # Final zone represents all devices joined from then on
                    
                    # Check if dynamic MDO (rendering method dependent)
                    if 'mdoDynamic' in obj:
                        OBJdynamic = int(obj['mdoDynamic'])
                    else:
                        OBJdynamic = 0 # Default static
                    OBJdynamic = bool(OBJdynamic)
                    
                    # Gain to apply to object if routed to MDO
                    if 'mdoGainDB' in obj:
                        OBJgain = dB2lin(float(obj['mdoGainDB']))
                    else:
                        OBJgain = 1.0   # Default no gain
                    
                    # Channel to route to if MDO device
                    if 'speakerNumber' in obj:
                        mdoId = int(obj['speakerNumber'])
                    else:
                        mdoId = 0   # Default no channel
                    
                    # Commented because dynamic always means something now
                    # Turn off dynamic rendering if don't know where speakers are (could be set to !=3)
                    #if OBJmethod == 1:
                    #    OBJdynamic = 0
                    
                    # Check if has previous channel routing data
                    if 'mdoPreviousId' in objHistory:
                        OBJprevious = int(objHistory['mdoPreviousId'])
                        # And check if know the level of the previous zone it was put in
                        if 'mdoPreviousZoneLevel' in objHistory:
                            OBJprevzone = int(objHistory['mdoPreviousZoneLevel'])
                        else:
                            OBJprevzone = -1
                    else:
                        OBJprevious = -1    # Default no previous channel routing
                        OBJprevzone = -1    # Or previous zone
                    
                    # Check if has any specified MDO zones (default none)
                    OBJzones = [0]*self.MaxZones
                    ###if 'mdoZones' in obj:
                    ###    for j in range(0,self.MaxZones):
                    ###        OBJzones[j] = int(obj['mdoZones'][str(j+1)])
                    for j in range(0,self.MaxZones):
                        zonename = 'zone'+str(j+1)
                        if zonename in obj:
                            OBJzones[j] = int(obj[zonename])
                            OBJzones[j] = OBJzones[j]-1		# Because metadata 'levels' received currently start from 1 not 0
                    
                    # Does object have any previous active speakers
                    anyPrevious = [j for j, x in enumerate(OBJprevious==MDOactive) if x]
                    isPrevious = len(anyPrevious) > 0
                    
                    # Print object details
                    if self.verbose:
                        print(objName,'- Method:',method_print,'; zone levels:',OBJzones,'; dynamic:',OBJdynamic)
                    
                    """ Perform rendering method """
                    
                    # No output id, zone or zone level until find one
                    op_id = -1
                    op_zone = -1
                    op_zone_level = -1
                    
                    if OBJmethod == 0:
                        
                        # Pass specific channel rendering method - find speaker routed to this channel
                        
                        for ls in self.loudspeakers:
                            if int(ls['id']) == mdoId and any(int(ls['id'])==MDOactive):
                                # Use this speaker if active MDO device with requested channel
                                op_id = int(ls['id'])
                                if self.verbose:
                                    print('%s - Direct routing to speaker id: %i' % (objName,op_id))
#                        op_zone = -1
#                        op_zone_level = -1
                        
                    elif OBJmethod == 1:
                        
                        # Unknown loudspeaker locations rendering method
                        
                        # Shouldn't get here but print warning if do and return no op_id
                        print('WARNING: shouldn''t get here - should switch to method 2 with assumed zones')                
#                        op_id = -1
#                        op_zone = -1
#                        op_zone_level = -1
                        
                    elif OBJmethod == 2:
                        
                        # Coarse zonal loudspeaker rendering method
                        
                        if (not self.LSupdate and isPrevious):
                            
                            # If there is no configuration update and object has previous active speaker - use previous speaker id
                            
                            op_id = OBJprevious
                            op_zone = int(MDOzone[anyPrevious[0]])        
                            op_zone_level = OBJprevzone
                            
                            if self.verbose:
                                print('%s - Keeping previous MDO speaker id: %i' % (objName,op_id))
                            
                        else:
                            
                            # Need to pick an active speaker based on zone...
                            
                            op_id,op_zone,op_zone_level = zone_method(MDOactive,MDOzone,NMDO,OBJzones,OBJprevious,OBJprevzone,OBJdynamic,anyPrevious,isPrevious,objName,self.verbose)
                            
                    elif OBJmethod == 3:
                        
                        # Known loudspeaker location rendering method
                        
                        if not self.LSupdate and isPrevious and not OBJdynamic:
                            
                            # If there is no configuration update, and object has previous active speaker, and object is static - use previous speaker id
                            
                            op_id = OBJprevious
#                            op_zone = -1
#                            op_zone_level = -1
                            
                            if self.verbose:
                                print('%s - Keeping previous MDO speaker id: %i' % (objName,op_id))
                            
                        else:
                            
                            # Need to pick an active speaker based on location...
                            
                            op_id = nearest_neighbour_method(obj,self.loudspeakers,MDOactive,i,self.verbose)
#                            op_zone = -1
#                            op_zone_level = -1
                    
                    # Tidy up for this object...
                    
                    # Set loudspeaker channel to route out on (if found valid id)
                    if op_id >= 0:
                        for ls in self.loudspeakers:
                            if int(ls['id']) == op_id:
                                # Output channel is that in op_id
                                obj['type'] = 'channel'
                                #obj['outputChannels'] = ls['channel']
                                obj['outputChannels'] = ls['id']
                                # Output gain is current gain with any MDO specific gains
                                if 'mdoGainDB' in ls:
                                    MDOgain = dB2lin(float(obj['mdoGainDB']))
                                else:
                                    MDOgain = 1.0 # Default no gain
                                obj['level'] = float(obj['level'])*OBJgain*MDOgain
                                # print(obj)
                                if self.verbose:
                                    if OBJmethod == 3:
                                        print('%s - Routed to MDO speaker id: %s ; channel: %s ; x: %d, y: %d, z: %d' % (objName,ls['id'],ls['channel'],float(ls['x']),float(ls['y']),float(ls['z'])))
                                    else:
                                        print('%s - Routed to MDO speaker id: %s ; channel: %s ; zone: %i at level: %i' % (objName,ls['id'],ls['channel'],op_zone,op_zone_level))
                    elif mdoOnlyThreshold:
                        obj['level'] = 0.0
                        objHistory['active'] = False 
                    else:
                        if self.verbose:
                            print('%s - no available MDO device' % (objName))
                    
                    # Store this routing id for next time
                    objHistory['mdoPreviousId'] = op_id
                    objHistory['mdoPreviousZoneLevel'] = op_zone_level
                    
                    # Update object history
                    self.objHistory[id] = objHistory
                    
                    if self.verbose:
                        print('')
            
            # Each period, increment the count
            if self.periods >= self.refreshPeriod:
                self.periods = 1
                self.verbose = self.verbose0	# Reset verbose state when gets back to beginning
            else:
                if self.periods == 1:
                    self.verbose = False	# Turn off verbose
                self.periods += 1
            # Tidy up...
            
        # Speaker routing has been updated - reset back to zero (note inside self.on loop to ensure LSupdate message received next time turned on)
        self.LSupdate = False
        #print(objectVector)
        
        # Reset and device history clearing flag
        self.LSclear = False
        
        # Return result
        return objectVector
        
    def datagramReceived(self, data, address ):
        
        """ Callback function that is called when data has been received through the UDP input port. """
        
        if self.verbose0:
            print( 'mdoProductionProcessor:datagramReceived(): %d bytes received' % len(data) )
        try:
            # Load JSON data
            allData = json.loads( data )
            # Write archive to file if requested
            if len(self.writeLsToFile) > 0:
                filename = self.writeLsToFile
                t = gmtime()
                filename = ('%s_%04i%02i%02i%02i%02i%02i.txt' % (filename,t.tm_year,t.tm_mon,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec))
                with open(filename, 'w') as file:
                    file.write(json.dumps(allData))
            # Get speaker data        
            self = getLsData(self,allData)
        except Exception as ex:
            print( "Error receiving data in mdoProductionProcessor: %s" % str(ex) )

    def setParameter( self, key, valueList ):
        
        """ Set the parameter to a given value."""

        if key == "on":
            if valueList[0] == 0 or valueList[0] == 1:
                self.on = bool(int(valueList[0]))
                print('MDO processor status: %s' % ['off','on'][self.on])
            else:
                raise KeyError( "Command \"on\" must be 0 or 1" )
            self.periods = 1
            #self.periods = self.refreshPeriod
            
#            if self.on:
#                self.periods = 50

#            # When an off message is recieved, send empty strings to each of the MDO loudspeaker content descriptions
#            if self.on == 0:
#                for l in self.loudspeakers:
#                    if 'mdo' in l and int(l['mdo']) == 1:
#                        oscaddress = "/speaker%i" % int(l['id'])
#                        send_loudspeaker_content("",oscaddress,'127.0.0.1',4555)


        elif key == "verbose":
            if valueList[0] == 0 or valueList[0] == 1:
                self.verbose0 = bool(int(valueList[0])) # Set verbose0 as this is what keeps the record of the verbose state
                self.periods = 1
                #self.periods = self.refreshPeriod
            else:
                raise KeyError( "Command \"verbose\" must be 0 or 1" )
            print("MDO processor verbose set to:",self.verbose0)
                
        elif key == "priority":
            self.priority = int(valueList[0])
            self.periods = 1
            #self.periods = self.refreshPeriod
            print("MDO processor priority set to: %i" % self.priority)
            
        elif key == "loudspeakerconfig":
            self.loudspeaker_config = valueList[0]
            self.loudspeakers = Parse_Loudspeaker_Config(self.loudspeaker_config)
            self.LSupdate = False                   # Stops the below adding old speaker data
            self = update_loudspeaker_data(self)    # Because converts some data types and prints
            self.periods = 1
            #self.periods = self.refreshPeriod
            print("MDO processor loudspeakers set with config:",self.loudspeaker_config)

        elif key == "loudspeakerswitch":      # Can turn individual MDO loudspeakers on and off
            self.periods = 1
            #self.periods = self.refreshPeriod

            switch_id = int(valueList[0])
            switch_status = int(valueList[1])
            
            for l in self.loudspeakers:                 # Look through the speakers
                if int(l['id']) == switch_id:           # If it's the matching ID
                    l['switch'] = switch_status         # Set it's status
                    print("MDO loudspeaker %i set to: %s" % (switch_id,switch_status))
#                    if switch_status == 0:
#                        oscaddress = "/speaker%i" % switch_id
#                        send_loudspeaker_content("", oscaddress, '127.0.0.1', 4555)

        #elif key == "reset":    # Reset the strings containing object names

        elif key == "method":
            self.method = int(valueList[0])
            self.periods = 1
            #self.periods = self.refreshPeriod
            print("MDO processor rendering method set to: %i" % self.method)
            
        elif key == "numberZones":
            self.MaxZones = int(valueList[0])
            self.periods = 1
            #self.periods = self.refreshPeriod
            print("MDO processor number of zones set to: %i" % self.MaxZones)
            
        elif key == "updateDevices":
            # self.LSupdate = bool(int(valueList[0]))
            self.LSupdate = True
            self.periods = 1
            #self.periods = self.refreshPeriod
            print("MDO processor: device update")
            
        elif key == "clearDeviceHistory":
            # self.LSclear = bool(int(valueList[0]))
            self.LSclear = True
            self.periods = 1
            #self.periods = self.refreshPeriod
            print("MDO processor: clear device history")
            
        elif key == "refreshPeriod":
            self.refreshPeriod = int(valueList[0])
            self.periods = 1
            #self.periods = self.refreshPeriod
            print("MDO processor refresh period set to: %i" % self.refreshPeriod)
        
        elif key == "readFromFile":
            # Read JSON string of settings from file
            filename = valueList[0]
            filename = str(filename)
            filename = filename[2:len(filename)-1]
            with open(filename, 'r') as file:
                self = getLsData(self,json.load(file))
            print('\nReading loudspeaker data from file: %s\n' % filename)
            
        else:
            raise KeyError( "MDOProcessor parameter set type unsupported")



        #if (key != "envelopment") or (len( valueList ) != 1) or (not isinstance(valueList[0], float) ):
        #    raise KeyError( "MDOProcessor supports only the parameter set command \"envelopment\" consisting of a single float value" )
        #self.env = valueList[0]
        #print "Envelopment set at %2.2f" % (self.env)

