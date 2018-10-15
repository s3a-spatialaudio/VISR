# edit reverb processor
# edits reverb metadata in the object stream based on recevied control messages

# libraries
from copy import deepcopy
from pprint import pprint
import json
import numpy
import math

from metadapter import SequenceProcessorInterface

# class definition
class EditDistanceProcessor(SequenceProcessorInterface):
  
  # The init function can be used to setup all internal data members.
  # The parameter "arguments" contains the XML configuration for this
  # processor as stored in the configuration file
  def __init__(self, arguments ):
    # Mandatory call to the base class initialisation method
    SequenceProcessorInterface.__init__(self, arguments)
      # Any definition and initialisation of data local to the object
      # goes here. This includes any consitency checking of the settings in
      # arguments.
    
    # initialize radius
    self.r = float(arguments.attrib['r'])
    
    # inititalise the list of obejcts to add reverb to
    self.editObjectList = eval(arguments.attrib['objectIds'])
    try:
      0 in self.editObjectList
    except TypeError:
      self.editObjectList = [self.editObjectList]

    else:
      print( "Edit distance error" )
    print( "distance modifications will be made to objects:" )
    print( self.editObjectList )


  # Metadata adaptation.
  # The method is called periodically from the adaptation engine.
  # The argument "objectVector" contains current object vector (carrying the adaptations made in the previous stage) as a Python list,
  # The function must return an object vector (can be either the modified input argument, a deep copy or a newly constructed object vector.)
  def processObjectVector( self, objectVector):
    # Iterate over all objects in the vector
    #objCopy=deepcopy(objectVector)
    #pprint(objectVector)
    for obj in objectVector:
      # Iterate through all objects in the vector.
      # First check that the source is a point source with reverb
      # Check whether the current obj is in the list of object IDs to edit
      if int(obj['id']) in self.editObjectList:
        if obj['type'] == "pointreverb":
        # Distance editing
          # direct level
          if not self.r ==0:
            directLevelAdjust = 1 / self.r
            obj['level']=float(obj['level'])* directLevelAdjust
          
            # early levels
            for reflection in obj['room']['ereflect']:
              reflection['level'] = float(reflection['level']) / (math.sqrt(self.r) * directLevelAdjust)
        
            # late level
            lateLevelsList = obj['room']['lreverb']['level']
            newlateLevelsList = []
            for lateBand in range(0,len(lateLevelsList.split(","))):
              newlateLevelsList.append(float(lateLevelsList.split(",")[lateBand]) / directLevelAdjust)
            
            obj['room']['lreverb']['level'] = str(newlateLevelsList)[1:-1]
            
          else:
            print( "relative radius cannot be zero" )
  
        else:
          # just edit the direct level
          if not self.r ==0:
            directLevelAdjust = 1 / self.r
            obj['level']=float(obj['level'])* directLevelAdjust
        
        #
      #
    # Return the object vector
    #print( "EDITED" )
    #pprint(objectVector)
    return objectVector
  
  
  
  
  
  # User defined method to handle incoming control data.
  # This method has to be implemented (overridden) if this processor class accepts external control data.
  # This method is called asynchronuously to the object adaptation because control data can arrive at arbitrary points in time. this implies that this method cannot directly influence the object vectors, but the received ontrol information has to be stored in the object's internal state (data members).
  # The argument 'key' is a string that denotes the type of a message. The meaning is specific to the processor and allows handling of multiple types of messages by a single object.
  # 'valueList' contains the parameters of this command as a Python list. Number, type, and semantics of the values are application-specific.
  # i.e. OSC messages contain \Address\key\valueList ?
  def setParameter( self, key, valueList ):
    
    # change the room library
    # OSC example: /EditDistance/r 1.1
    if key == "r":
      self.r = float(valueList[0])
    elif key == "objectlist":
      # change the objects to apply distance to, currently requires full object list per message
      editDistanceString = valueList[0]
      if editDistanceString == "all":
        maxObjects=64
        self.editObjectList = []
        for i in range(0,maxObjects):
          self.editObjectList.append(i)
      elif editDistanceString == "none":
        self.editObjectList = []
      elif editDistanceString == "list":
        self.editObjectList =  valueList[1:]
        try:
          0 in self.editObjectList
        except TypeError:
          self.editObjectList = [self.editObjectList]
      else:
        print( "REVERB ERROR: initObjs string must be 'all', 'none' or 'list (with a corresponding comma separated initObjectList)'" )
    else:
      raise KeyError("EditDistanceProcessor supports keys editearlylevel, editlatelevel, editlatedecay")
