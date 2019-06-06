# -*- coding: utf-8 -*-
"""
Created on Mon 21 Jan 2018

@author: Andreas Franck a.franck@soton.ac.uk
"""

from metadapter import SequenceProcessorInterface

#from twisted.internet.protocol import DatagramProtocol
#from twisted.internet import reactor

import json
from time import gmtime

#from sys import version_info
#
#if version_info.major <= 2:
#    import OSC
#else:
#    # Use the self-made port for Python 3 (experimental)
#    import thirdparty.OSC as OSC

def objPersistent(objNew,self):
    """ Internal function to ensure persistence of object data """
    result = []
    self.objHistory.extend(objNew)
    for obj in self.objHistory:
        if obj not in result:
            current_id = False
            for res in result:
                if res['id'] == obj['id']:
                    res.update(obj)
                    current_id = True
            if not current_id:
                result.append(obj)
    return result

def getJsonData(self,data,suffix):

    """ Internal function to save to file and determine message type """
    if 'mdoObjects' in data:  # Legacy term
        suffix = 'objects'
        # Get object metadata
        allData = data["mdoObjects"]
    elif 'mdoGroups' in data: # Legacy term
        suffix = 'groups'
        # Get groups metadata
        allData = data["mdoGroups"]
    else:
        # Data assumed to be of type 'suffix'
        allData = data

    # Write data to file for archiving
    if len(self.writeToFile) > 0:
        filename = self.writeToFile
        t = gmtime()
        # filename = ('%s_%s%04i%02i%02i%02i%02i%02i.txt' % (filename,suffix,t.tm_year,t.tm_mon,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec))
        filename = ('%s_%s%04i%02i%02i%02i%02i.txt' % (filename,suffix,t.tm_year,t.tm_mon,t.tm_mday,t.tm_hour,t.tm_min))
        with open(filename, 'w') as file:
            file.write(json.dumps(allData))
    # Update object data
    self = getObjData(self,allData,suffix)
    
    return(self)

def getObjData(self,allData,dataType):

    """ Internal function to get object data from dict """
    # Extract the actual table.
    # Depends on the actual send format.
    # Get the mode (update/overwrite)
    
	#if 'mode' in allData:
    #    mode = allData["mode"]
    #else:
    #    mode = self.updateMode # Use default
    mode = self.updateMode # Use default
	
    if dataType == 'objects':
        # Store the received data in a lookup table (a dict) with the id as search index.
        # Note: Revisit whether the ID should be transmitted as "ID"
        if mode=='overwrite':
            self.objectData = {}
        for obj in allData:
            if 'id' in obj:
                objId = int(float(obj.pop( "id" )))
            elif 'objectNumber' in obj:
                objId = int(float(obj.pop( "objectNumber" )))-1
            if 'groupNumber' in obj:
                grpId = int(float(obj.pop( "groupNumber" )))
            elif 'group' in obj:
                grpId = int(float(obj.pop( "group" )))
            else:
                grpId = 0
            obj['group'] = grpId
            # Check if has updated
            if objId in self.objectData and self.objectData[objId] == obj:
                hasUpdated = 0
            else:
                self.objectData[objId] = obj
                hasUpdated = 1
            self.objectData[objId].update({'objectUpdate':hasUpdated})
        # Unfortunately this does not work, because id is removed before it can be used for the index
        # int(obj["ID"]): obj.pop("ID") for obj in tableData }
    elif dataType == 'groups':
        if mode=='overwrite':
            self.groupData = {}
        for group in allData:
            if 'groupNumber' in group:
                grpId = int(float(group.pop( "groupNumber" )))
            elif 'group' in group:
                grpId = int(float(group.pop( "group" )))
            if grpId in self.groupData and self.groupData[grpId] == group:
                hasUpdated = 0
            else:
                self.groupData[grpId] = group
                hasUpdated = 1
            self.groupData[grpId].update({'objectUpdate':hasUpdated})
    return self

#class TableReceiver( DatagramProtocol ):
#    """ Internal class to handle receit of table messages """
#    def __init__( self, parent, verbose=False ):
#        super(TableReceiver).__init__()
#        self.parent = parent
#        self.verbose = verbose
#
#    def startProtocol( self ):
#        if self.verbose:
#            print( 'NetworkUdpReceiver.startProtocol()' )
#            # Nothing really to do here (for a receiver)
#    def datagramReceived( self, data, address ):
#        self.parent.datagramReceived( data, address )

class AddObjectMetadataProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)
        
        # Get UDP ports for receiving object metadata on
        if "udpPort" in arguments.attrib:
            self.udpPort = int(arguments.attrib[ "udpPort" ])
        else:
            self.udpPort = 9001
            
        if "verbose" in arguments.attrib:
            self.verbose =  bool(arguments.attrib[ "verbose" ])
        else:
            self.verbose = False

        if "messagePacketSize" in arguments.attrib:
            inputPacketSize =  int(arguments.attrib[ "messagePacketSize" ])
        else:
            inputPacketSize = None
        
        if "writeToFile" in arguments.attrib:
            self.writeToFile =  arguments.attrib[ "writeToFile" ]
        else:
            self.writeToFile = ''
        
        if "updateMode" in arguments.attrib:
            self.updateMode = arguments.attrib[ "updateMode" ]
        else:
            self.updateMode = 'update'
        
        # Set up receiver for object/group metadata
        #self.objReceiver = TableReceiver( self, verbose = True )
        #if inputPacketSize == None:
        #    reactor.listenUDP( self.udpPort, self.objReceiver )
        #else:
        #    reactor.listenUDP( self.udpPort, self.objReceiver, maxPacketSize=inputPacketSize )
        
        self.objectData = {}
        self.groupData = {}
        self.objHistory = []

    def processObjectVector( self, objectVector):

#        self.objHistory = objPersistent(objectVector,self)
#        for obj in self.objHistory:
#            objId = int(obj["id"])
#            if objId in self.objectData:
#                obj.update( self.objectData[objId] )
#        return self.objHistory
        
        # Add additional metadata to object stream
        for obj in objectVector:
            # Object id
            objId = int(obj["id"])
            # Strip objectUpdate from object if there from last time
            if 'objectUpdate' in obj:
                obj.pop('objectUpdate')
            # If data exists object data, add to object stream
            if objId in self.objectData:
                obj.update( self.objectData[objId] )
                # Strip objectUpdate from internal data (for next time)
                if 'objectUpdate' in self.objectData[objId]:
                    self.objectData[objId].pop('objectUpdate')
            # Group id (zero means no group)
            if 'groupNumber' in obj:
                grpId = int(obj["groupNumber"])
            elif 'group' in obj:
                grpId = int(obj["group"])
            else:
                grpId = 0
            # If data exists in group data, add to object stream (overwriting object data)
            if grpId in self.groupData:
                obj.update( self.groupData[grpId] )
                # Strip objectUpdate from internal data (for next time)
                if 'objectUpdate' in self.groupData[grpId]:
                    self.groupData[grpId].pop('objectUpdate')
            else:
                obj['group'] = 0  # Say not in group if it doesn't exist
        return objectVector

    def datagramReceived(self, data, address ):
        """ Callback function that is called when data has been received through the UDP input port. """
        if self.verbose:
            print( 'AddObjectMetadata:datagramReceived(): %d bytes received' % len(data) )
        try:
            if type(data) == bytes:
                data = data.decode("utf-8")
            data = json.loads( data )
            getJsonData(self,data)
        except Exception as ex:
            print( "Error receiving data in AddObjectMetadata: %s" % str(ex) )

    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        if key == "verbose":
            if valueList[0] == 0 or valueList[0] == 1:
                self.verbose = bool(int(valueList[0]))
            else:
                raise KeyError( "Command \"verbose\" must be 0 or 1" )
        elif key == "updateMode":
            if valueList[0] == 0 or valueList[0] == 1:
                self.updateMode = int(valueList[0])
                if self.updateMode==0:
                    self.updateMode = 'update'
                else:
                    self.updateMode = 'overwrite'
            else:
                raise KeyError( "Command \"updateMode\" must be 0 or 1" )
        elif key == "objectReset":
            # Clear object history (currently not used?)
            self.objHistory = []
            print('\nObject history reset\n')
        elif key == "readFromFile":
            # Read JSON string of settings from file
            filename = valueList[0]
            filename = str(filename)
            filename = filename[2:len(filename)-1]
            with open(filename, 'r') as file:
                self = getObjData(self,json.load(file))
            print('\nReading object data from file: %s\n' % filename)
        elif key == "objects" or key == "groups":
            # Received object/group metadata update
            print( 'AddObjectMetadata:OSC/JSON control:%s package received ' % key)
            getJsonData(self, valueList, key)
        else:
            raise KeyError( "Unrecognised key in AddObjectMetadata processor")

            
        #pass # No control data mechanism yet, as JSON data is received through a dedicated network port.