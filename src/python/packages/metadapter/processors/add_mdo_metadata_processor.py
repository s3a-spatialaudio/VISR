# -*- coding: utf-8 -*-
"""
Created on Mon 21 Jan 2018

@author: Andreas Franck a.franck@soton.ac.uk
"""

from metadapter import SequenceProcessorInterface

from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor

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

def getObjData(self,allData):
    """ Internal function to get object data from dict """
    # Extract the actual table.
    # Depends on the actualsend format.
    if 'mdoObjects' in allData:
        tableData = allData["mdoObjects" ]
        # Store the received data in a lookup table (a dict) with the id as search index.
        # Note: Revisit whether the ID should be transmitted as "ID"
        self.mdoData = {}
        for obj in tableData:
            #objId = int(obj.pop( "ID" ))
            if 'id' in obj:
                objId = int(obj.pop( "id" ))
            elif 'objectNumber' in obj:
                objId = int(obj.pop( "objectNumber" ))-1
            self.mdoData[objId] = obj
        # Unfortunately this does not work, because id is removed before it can be used for the index
        # int(obj["ID"]): obj.pop("ID") for obj in tableData }

    return self

class TableReceiver( DatagramProtocol ):
    """ Internal class to handle receit of table messages """
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

class AddMdoMetadataProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)

        self.udpPort = int(arguments.attrib[ "udpPort" ])

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

        self.udpReceiver = TableReceiver( self, verbose = True )
        if inputPacketSize == None:
            reactor.listenUDP( self.udpPort, self.udpReceiver )
        else:
            reactor.listenUDP( self.udpPort, self.udpReceiver, maxPacketSize=inputPacketSize )

        self.mdoData = {}
        self.objHistory = []

    def processObjectVector( self, objectVector):

#        self.objHistory = objPersistent(objectVector,self)
#        for obj in self.objHistory:
#            objId = int(obj["id"])
#            if objId in self.mdoData:
#                obj.update( self.mdoData[objId] )
#        return self.objHistory

        for obj in objectVector:
            objId = int(obj["id"])
            if objId in self.mdoData:
                obj.update( self.mdoData[objId] )
        return objectVector

    def datagramReceived(self, data, address ):
        """ Callback function that is called when data has been received through the UDP input port. """
        if self.verbose:
            print( 'AddMdoMetadata:datagramReceived(): %d bytes received' % len(data) )
        try:
            allData = json.loads( data )
            # Write data to file for archiving
            if len(self.writeToFile) > 0:
                filename = self.writeToFile
                t = gmtime()
                filename = ('%s_%04i%02i%02i%02i%02i%02i.txt' % (filename,t.tm_year,t.tm_mon,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec))
                with open(filename, 'w') as file:
                    file.write(json.dumps(allData))
            # Update object data
            self = getObjData(self,allData)

        except Exception as ex:
            print( "Error receiving data in AddMdoDataProcessor: %s" % str(ex) )

    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        if key == "objectReset":
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
        else:
            raise KeyError( "Unrecognised key in add mdo metadata processor")


        #pass # No control data mechanism yet, as JSON data is received through a dedicated network port.