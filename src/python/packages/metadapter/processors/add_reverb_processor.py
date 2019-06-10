# add reverb processor
# adds reverb metadata to the object stream based on recevied control messages

# libraries
from copy import deepcopy
from pprint import pprint
import json
import numpy

from metadapter import SequenceProcessorInterface

# Method to get the "room" structure from a reverb library entry
def addReverbMetadataToObject(obj,reverbLibrary,roomName):
  foundRoom=False
  #print(roomName)
  reverbLibraryCopy = deepcopy(reverbLibrary)
  for libEntry in reverbLibraryCopy:
    if roomName==libEntry['name']:

      foundRoom=True
      obj['room']=libEntry['room']
      obj['type'] = "pointreverb"
      # rotate the azimuth by comparing the object position with the reverb library position
      if 'x' in obj['position']:
        x_pos = float(obj['position']['x'])
        y_pos = float(obj['position']['y'])
        azimuth_deg = numpy.arctan2(y_pos,x_pos) * 180 / numpy.pi
      else:
        azimuth_deg = obj['position']['el']
      rotationValue = azimuth_deg - float(libEntry['position']['az'])
      # iterate through the early reflections to apply the rotation
      iRefl = 0
      for earlyRefl in obj['room']['ereflect']:
        obj['room']['ereflect'][iRefl]['position']['az'] = float(earlyRefl['position']['az']) + rotationValue
        iRefl = iRefl + 1

  if not foundRoom:
    print( "room is not in the reverb library" )

  return obj

# class definition
class AddReverbProcessor(SequenceProcessorInterface):

  # The init function can be used to setup all internal data members.
  # The parameter "arguments" contains the XML configuration for this
  # processor as stored in the configuration file
  def __init__(self, arguments ):
    # Mandatory call to the base class initialisation method
    SequenceProcessorInterface.__init__(self, arguments)
      # Any definition and initialisation of data local to the object
      # goes here. This includes any consitency checking of the settings in
      # arguments.

    # load the reverb object library file
    fp = open(arguments.attrib['libFile'])
    self.reverbLibrary = json.load(fp)['roomlibrary']
    self.nRooms = len(self.reverbLibrary)
    print( "%d rooms found in reverb library" % (self.nRooms) )
    #pprint(self.reverbLibrary)

    # define and intialise dictionary for room library
    self.roomName = arguments.attrib['initRoomName']

    # inititalise the list of obejcts to add reverb to
    initReverbString = arguments.attrib['initObjs']
    if initReverbString == "all":
      maxReverbObjects=64
      self.reverbObjectList = []
      for i in range(0,64):
        self.reverbObjectList.append(i)
    elif initReverbString == "none":
      self.reverbObjectList = []
    elif initReverbString == "list":
      self.reverbObjectList = eval(arguments.attrib['initObjectList'])
      if type(self.reverbObjectList) == int:
        self.reverbObjectList=[self.reverbObjectList]
      try:
        0 in self.reverbObjectList
      except TypeError:
        self.reverbObjectList = [self.reverbObjectList]

    else:
      print( "REVERB ERROR: initObjs string must be 'all', 'none' or 'list (with a corresponding comma separated initObjectList)'" )
    print( "reverb will be added to objects:" )
    print( self.reverbObjectList )


  # Metadata adaptation.
  # The method is called periodically from the adaptation engine.
  # The argument "objectVector" contains current object vector (carrying the adaptations made in the previous stage) as a Python list,
  # The function must return an object vector (can be either the modified input argument, a deep copy or a newly constructed object vector.)
  def processObjectVector( self, objectVector):
    # Iterate over all objects in the vector
    for obj in objectVector:
      # Iterate through all objects in the vector.
      # First check that the source is a point source
      if obj['type'] == "point":
        # Reverb adaptation
        # Check whether the current obj is in the list of object IDs to add reverb to and if so create a "room" entry
        if int(obj['id']) in self.reverbObjectList and not self.roomName=="none":
          obj = addReverbMetadataToObject(obj,self.reverbLibrary,self.roomName)

    # Return the object vector
    pprint(objectVector)
    return objectVector





  # User defined method to handle incoming control data.
  # This method has to be implemented (overridden) if this processor class accepts external control data.
  # This method is called asynchronuously to the object adaptation because control data can arrive at arbitrary points in time. this implies that this method cannot directly influence the object vectors, but the received ontrol information has to be stored in the object's internal state (data members).
  # The argument 'key' is a string that denotes the type of a message. The meaning is specific to the processor and allows handling of multiple types of messages by a single object.
  # 'valueList' contains the parameters of this command as a Python list. Number, type, and semantics of the values are application-specific.
  # i.e. OSC messages contain \Address\key\valueList ?
  def setParameter( self, key, valueList ):

    # change the room library
    # OSC example: /AddReverb/roomname church
    if key == "roomname":
      self.roomName = bytes.decode(valueList[0])
      #print(self.roomName)

    # OSC examples: /AddReverb/objectlist all
    #             : /AddReverb/objectlist none
    #             : /AddReverb/objectlist 0 1 10 11
    elif key == "objectlist":
      # change the objects to apply reverb to, currently requires full object list per message
      initReverbString = valueList[0]
      if initReverbString == "all":
        maxReverbObjects=64
        self.reverbObjectList = []
        for i in range(0,64):
          self.reverbObjectList.append(i)
      elif initReverbString == "none":
        self.reverbObjectList = []
      elif initReverbString == "list":
        self.reverbObjectList =  valueList[1:]
        try:
          0 in self.reverbObjectList
        except TypeError:
          self.reverbObjectList = [self.reverbObjectList]

      else:
        print( "REVERB ERROR: initObjs string must be 'all', 'none' or 'list (with a corresponding comma separated initObjectList)'" )
      print( "reverb will be added to objects:" )
      print( self.reverbObjectList )

    else:
      raise KeyError("AddReverbProcessor supports keys roomname and objectlist")
