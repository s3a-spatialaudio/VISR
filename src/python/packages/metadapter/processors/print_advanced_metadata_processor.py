# Read advanced metadata xml chunk from BWF/ADM file and print result
# Richard Hughes, University of Salford, r.j.hughes@salford.ac.uk

from distutils.util import strtobool

from metadapter import SequenceProcessorInterface

# Required for the example class, but not for the template.
import numpy

from copy import deepcopy
import math
import xml.etree.ElementTree as ET


def rad2deg( w ):
	return 180.0/math.pi * w

def deg2rad( w ):
	return math.pi/180.0 * w

def cart2sphDeg(x,y,z):
	XsqPlusYsq = x**2 + y**2
	r = math.sqrt(XsqPlusYsq + z**2)               	# r
	elev = math.atan2(z,math.sqrt(XsqPlusYsq))     	# theta
	az = math.atan2(y,x)                           	# phi
	return r, rad2deg(az), rad2deg(elev)

def sphDeg2cart(r,az,elev):
	az = deg2rad(az)
	elev = deg2rad(elev)
	x = r*math.cos(az)*math.cos(elev)		   		# x
	y = r*math.sin(az)*math.cos(elev)		   		# y
	z = r*math.sin(elev)			   				# z
	return x, y, z

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

def parse_xml_scene(tag,tag_target,xml_string,attr,attr_defaults):

	# Parse scene data from xml

	# Get info from xml string
	root = ET.fromstring(xml_string)

	# Initilaise attributes of tag and defaults
	tag = {}
	for j in range(0,len(attr)):
		tag[attr[j]] = attr_defaults[j]

	# Get xml results
	for child in root:
		if child.tag == tag_target: # Found target tag
			for x in tag:
				if x in child.attrib:
					tag[x] = child.attrib[x]

	return tag

def parse_xml_objects(tag,tag_target,xml_string,Nobj,attr,attr_defaults):

	# Parse object data from xml

	# Get info from xml string
	root = ET.fromstring(xml_string)

	# Initilaise attributes of tag and defaults
	tag = []
	for i in range(0,Nobj):
		tag.append(dict(id=i))
		for j in range(0,len(attr)):
			tag[i][attr[j]] = attr_defaults[j]

	# Get xml results sort into correct order by ID
	for child in root:
		if child.tag == tag_target: # Found target tag
			# Get ID
			if 'id' in child.attrib:
				id = int(child.attrib['id'])
			else:
				print ("error - missing ID")
				return
			for x in tag[id]:
				if x in child.attrib:
					tag[id][x] = child.attrib[x]

	return tag

class PrintAdvancedMetadataProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
      SequenceProcessorInterface.__init__(self, arguments)
      
      # Get filename to load
      filename = arguments.attrib['filename']

      # Get active flag (not actually used here)
      self.active = strtobool( arguments.attrib['active'] )

      # Read advanced metadata chunk from file
      target_chunk = "bxml"
      (wav_string,fmt) = parse_WAV_chunk(filename,target_chunk)

      # Settings for advanced scene metadata to find
      self.scene = []								# Create empty list for results
      tag_target = 'scene'						# Target tag
      attr = ['scene_attr_1','scene_attr_2']		# Attributes to try and find within tag
      attr_defaults = [0,'option0']				# Defaults for attributes for when not found

      # Parse wav_string assuming scene data in xml target tag
      self.scene = parse_xml_scene(self.scene,tag_target,wav_string,attr,attr_defaults)

      # Print to screen
      print()
      print()
      print("XML parse attributes for tag = '{}':\n".format(tag_target))
      for x in self.scene:
        print ("{:-12s}\t: {}".format(x,str(self.scene[x])))
      print()

      # Settings for advanced object metadata to find
      self.objects = []							# Create empty list for results
      tag_target = 'object'						# Target tag
      Nobj = fmt['Nchan']							# Number of objects (assumed same as number of channels in file)
      attr = ['object_attr_1','object_attr_2']	# Attributes to try and find within tag
      attr_defaults = [0,'option0']				# Defaults for attributes for when not found

      # Parse wav_string assuming set of object data in xml target tag
      self.objects = parse_xml_objects(self.objects,tag_target,wav_string,Nobj,attr,attr_defaults)

      # Print to screen
      print ("\nXML parse attributes for tag = '{}':\n".format(tag_target))
      for x in self.objects:
        for y in x:
          print ("{:-12s}\t: {}".format(y,str(x[y])))
        print()
      print()

    def processObjectVector( self, objectVector):

      # Use self.objects and self.scene here, e.g.
      if self.active:
        # Example scene attribute logic
        if self.scene['scene_attr_1'] == '1':
          print ('Scene attribute condition is TRUE')
        else:
          print ('Scene attribute condition is FALSE')
        # Example object attribute logic
        if self.objects[0]['object_attr_2'] == 'option1':
          print ('Object attribute condition is TRUE')
        else:
          print ('Object attribute condition is FALSE')

      return objectVector

    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        if (key == "active"):
            if len( valueList ) != 1:
                raise KeyError( "PrintAdvancedMetadataProcessor command \"active\" must contain 1 value." )
            self.active = bool( valueList[0] )
