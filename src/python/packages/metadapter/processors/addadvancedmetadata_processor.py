# -*- coding: utf-8 -*-
"""
Created on Tue Mar 21 2017

@author: Jon Francombe based on code by Rick Hughes
"""

from metadapter import SequenceProcessorInterface

import xml.etree.ElementTree as ET

from sys import version_info

if version_info.major <= 2:
    import OSC
else:
    # Use the self-made port for Python 3 (experimental)
    from ..thirdparty import OSC as OSC

def uint32(string):
    # Get uint32 value from upto length 4 string
    # (couldn't find built in way of doing this!!)
    base = int(2**8)
    N = len(string)
    if N > 4:
        print ("uint32 error")
        return
    num = int(0)
    for i in range(N-1,-1,-1):
        num=num+int(ord(string[i])*base**i)
    return num

def parse_WAV_chunk(filename,chunkname):

    # Find chunk in .wav file as string and some basic file details

    # Open wav file
    f = open(filename,"r")

    # Start at 0 in file
    file_pos = 0

    # Read RIFF header
    if f.read(4) != "RIFF":
        print ("RIFF chunk error")
        f.close()
        return
    else:
        file_pos = file_pos+4

    # Get total file length
    file_length = uint32(f.read(4))+8.0
    file_pos = file_pos+4

    # Read WAVE
    if f.read(4) != "WAVE":
        print ("WAVE chunk error")
        f.close()
        return
    else:
        file_pos = file_pos+4

    # Loop round finding chunks until get target
    chunk_string = ''
    while file_pos < file_length:
        chunki = f.read(4)
        chunki = chunki.strip()
        sizei = uint32(f.read(4))
        #print chunki
        #print sizei
        if chunki == chunkname:
            chunk_string = f.read(sizei)
            file_pos = file_length
        else:
            if chunki == 'fmt':
                # Get basic file details
                f.read(2)	# Skip 2 bytes
                fmt = {'Nchan':0,'fs':0}
                fmt['Nchan'] = uint32(f.read(2))	# Number of channels
                fmt['fs'] = uint32(f.read(4))		# Sample rate (Hz)
            sizei = sizei+sizei%2
            file_pos = file_pos+8+sizei
            f.seek(file_pos)

    # Close file
    f.close()

    # Return string found in chunk and fmt chunk file details
    return chunk_string, fmt


def send_target_envelopment(target_envelopment,osc_address,host,port):
    c = OSC.OSCClient()
    c.connect((host, port))
    oscmsg = OSC.OSCMessage()
    oscmsg.setAddress(osc_address)

    if target_envelopment == 'none':
        oscmsg.append(target_envelopment)
    else:
        oscmsg.append(float(target_envelopment))

    c.send(oscmsg)


def send_loudspeaker_content(string,osc_address,host,port):
    c = OSC.OSCClient()
    c.connect((host, port))
    oscmsg = OSC.OSCMessage()
    oscmsg.setAddress(osc_address)

    oscmsg.append(string)

    c.send(oscmsg)


def action_scene_metadata(advanced_metadata):
    # Function for acting on the values of overall scene metadata

    if 'target_envelopment' in advanced_metadata['scene']:
        target_envelopment_value = advanced_metadata['scene']['target_envelopment']
        send_target_envelopment(target_envelopment_value, '/target_envelopment', '127.0.0.1', 4501)
        send_target_envelopment(target_envelopment_value, '/EnvOpt/target_env', '127.0.0.1', 4240)

    else:
        send_target_envelopment('none', '/target_envelopment', '127.0.0.1', 4501)


def read_adm_file(filename, verbose):

    # First, send empty messages to the loudspeaker content strings
    send_loudspeaker_content(" ","/reset","127.0.0.1",4555)

    # Create a list of dictionaries for object metadata
    object_metadata = []

    # Create a dictionary of scene metadata
    scene_metadata = {}

    # Read advanced metadata chunk from file (if a file is specified)
    if filename != 'None':

        if verbose:
            print("")
            print("Reading advanced metadata from: %s" % filename)

        (wav_string, fmt) = parse_WAV_chunk(filename, "bxml")

        if wav_string != '':

            # Get XML data
            root = ET.fromstring(wav_string)


            for child in root:  # For each line of the advanced metadata

                if child.tag == 'scene':  # If it's scene metadata
                    for key, value in child.items():

                        # Add to dictionary
                        scene_metadata[key] = value

                        if verbose:
                            print("    Scene metadata has attribute %s with value %s" % (key, value))

                        # Can add sections here that handle different types of scene metadata
                        # ^ NOTE: This is now done in one function call: action_scene_metadata


                elif child.tag == 'object':  # If it's an object, add each attribute to the corresponding object metadata
                    object_metadata.append(child.attrib)  # Add the advanced metadata to the object_metadata dictionary

        else:
            print("    No advanced metadata found in file")

    else:

        if verbose:
            print("")
            print("Filename set to none. Not attempting to read any advanced metadata.")

    if verbose:
        print("")

    # Compile into one dictionary for output
    advanced_metadata = {'objects': object_metadata, 'scene': scene_metadata}

    # Act on scene metadata
    action_scene_metadata(advanced_metadata)

    return advanced_metadata



class AddAdvancedMetadataProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)

        if 'filename' in arguments.attrib:
            self.filename = str(arguments.attrib['filename'])
        else:
            self.filename = 'None'

        if 'verbose' in arguments.attrib:
            self.verbose = int(arguments.attrib['verbose'])
        else:
            self.verbose = 1    # Default is verbose

        if 'on' in arguments.attrib:
            self.on = int(arguments.attrib['on'])
        else:
            self.on = 1         # Default is on

        # Read the specified ADM file
        self.advanced_metadata = read_adm_file(self.filename, self.verbose)

        # Preallocate empty list of metadata to add
        self.metadata_to_add = []

        # Create reset flag
        self.resetflag = 0


    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.

        if self.on:

            if self.resetflag:
                self.resetflag = 0
                del objectVector
                objectVector = []
                #print("Just reset object vector. Length now: %i" % len(objectVector))


            for obj in objectVector:

                # Get the object ID
                current_id = int(obj['id'])

                #print ("In add advanced metadata, object %i" % current_id)

                # Find the object dictionary for this object (if it exists)
                object_index = -1

                #print("Length of advanced metadata: %i" % len(self.advanced_metadata['objects']))
                #print("Length of object vector: %i" % len(objectVector))

                # Initialise
                for i, object in enumerate(self.advanced_metadata['objects']):                   # Go through the dictionary

                    if 'id' in object and current_id == int(object['id']):          # If it has an ID field, and it matches one of the objects:
                        object_index = i                                            #   store the object that it matches


                # In the case that the ID wasn't found in the objects, it will be skipped
                if object_index == -1:
                    #print("Skipping object %i" % current_id)
                    continue

                #print("Current object: %i.  Object index (index for the dictionary containing details): %i" % (current_id,object_index))

                # Go through the fields in the dictionary and add them to the object vector
                for key, value in self.advanced_metadata['objects'][object_index].items():

                    if key != 'id':     # Don't add the ID — assume it already exists. Note any other duplicate fields will be overwritten, currently without warning
                        # TODO: Could add warning if a field that already exists is overwritten
                        # TODO: There could be other fields that we always ignore (e.g. long descriptive bits of text could cause message length problems)
                        obj[key] = value

                # Add any extra metadata (through parameter input) if it's present
                newmd = [md for md in self.metadata_to_add if md['object_id'] == current_id]

                for md in newmd:
                    obj[md['amd_key']] = md['amd_value']

                #if self.verbose:
                #print obj

        return objectVector


    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""

        onstatus = ['off', 'on']

        if key == "filename":

            # Remove the existing advanced metadata
            del self.advanced_metadata
            del self.metadata_to_add
            self.metadata_to_add = []

            # Set the filename
            self.filename = valueList[0]

            # Read the file
            self.advanced_metadata = read_adm_file(self.filename, self.verbose)

            # Set the reset flag (clears object vector)
            self.resetflag = 1

        elif key == "on":
            if valueList[0] == 0 or valueList[0] == 1:
                self.on = valueList[0]
                if self.verbose:
                    print("AddAdvancedMetadata processor is %s" % onstatus[self.on])
            else:
                raise KeyError( "Command \"on\" must be 0 or 1" )

        elif key == "object_metadata":

            # TODO: Could have some way of resetting values that have been overwritten back to the file AMD (i.e. removing from the dictionary)

            # Create dictionary of the new metadata
            d = {'object_id': valueList[0], 'amd_key': valueList[1], 'amd_value': valueList[2]}

            # Add to the list of dictionarys
            self.metadata_to_add.append(d)


        else:
            raise KeyError( "AddAdvancedMetadata processor supports only the parameter set commands \"on\" and \"filename\"")
