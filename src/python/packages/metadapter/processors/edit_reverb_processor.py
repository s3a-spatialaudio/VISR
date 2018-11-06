# edit reverb processor
# edits reverb metadata in the object stream based on recevied control messages

# libraries
from copy import deepcopy
from pprint import pprint
import json
import numpy
import math

from metadapter import SequenceProcessorInterface

def lin2dB( linVal ):
    return 20.0 * math.log10( linVal )
    
def dB2lin( dbVal ):
    return math.pow( 10.0, 0.05 * dbVal )

# class definition
class EditReverbProcessor(SequenceProcessorInterface):
  
  # The init function can be used to setup all internal data members.
  # The parameter "arguments" contains the XML configuration for this
  # processor as stored in the configuration file
  def __init__(self, arguments ):
    # Mandatory call to the base class initialisation method
    SequenceProcessorInterface.__init__(self, arguments)
      # Any definition and initialisation of data local to the object
      # goes here. This includes any consitency checking of the settings in
      # arguments.
    
    # initialize edit reverb values
    self.earlyLevelAdjust = float(arguments.attrib['earlyLevelAdjust'])
    self.lateLevelAdjust = float(arguments.attrib['lateLevelAdjust'])
    # [float(i) for i in levelstr.split(",")]
    self.lateSubbandLevelAdjust = [float(iBand) for iBand in arguments.attrib['lateSubbandLevelAdjust'].split(",") ]
    self.lateDecayAdjust = float(arguments.attrib['lateDecayAdjust'])
    self.preDelayAdjust = float(arguments.attrib['preDelayAdjust'])
    self.preDelayAdjustLate = float(arguments.attrib['preDelayAdjustLate'])
    self.lateDelayAdjust = float(arguments.attrib['lateDelayAdjust'])
    
    if not len(self.lateSubbandLevelAdjust) == 9:
      print( "[load] Subband level adjustment must be specified in 9 subbands" )
    
    if self.preDelayAdjust<0:
      print( "PreDelay cannot be less than 0 s" )
    
    # inititalise the list of obejcts to add reverb to
    self.editObjectList = eval(arguments.attrib['objectIds'])
    try:
      0 in self.editObjectList
    except TypeError:
      self.editObjectList = [self.editObjectList]

    else:
      print( "REVERB EDIT_ERROR" )

  # Metadata adaptation.
  # The method is called periodically from the adaptation engine.
  # The argument "objectVector" contains current object vector (carrying the adaptations made in the previous stage) as a Python list,
  # The function must return an object vector (can be either the modified input argument, a deep copy or a newly constructed object vector.)
  def processObjectVector( self, objectVector):
    # Iterate over all objects in the vector
    #objCopy=deepcopy(objectVector)
    #print( "INPUT" )
    #pprint(objectVector)
    for obj in objectVector:
      # Iterate through all objects in the vector.
      # First check that the source is a point source with reverb
      if obj['type'] == "pointreverb":
        # Reverb editing
        # Check whether the current obj is in the list of object IDs to edit
        if int(obj['id']) in self.editObjectList:
          # edit early levels and pre delay
          for reflection in obj['room']['ereflect']:
            reflection['level'] = float(reflection['level']) * dB2lin(self.earlyLevelAdjust)
            if not self.preDelayAdjust < 0:
              reflection['delay'] = float(reflection['delay']) + self.preDelayAdjust
            else:
              print( "PreDelay cannot be less than 0 s" )
          
          
          # late predelay (to match first early reflection)
          obj['room']['lreverb']['delay'] = float(obj['room']['lreverb']['delay']) + self.preDelayAdjustLate
          #print( self.preDelayAdjustLate )
          #print( obj['room']['lreverb']['delay'] )
          
          # edit late level and decay
          lateLevelsList = obj['room']['lreverb']['level']
          newlateLevelsList = []
          lateDecaysList = obj['room']['lreverb']['decayconst']
          newlateDecaysList = []
          lateDelaysList = obj['room']['lreverb']['attacktime']
          newlateDelaysList = []
          if len(self.lateSubbandLevelAdjust) == 9:
            for lateBand in range(0,len(lateLevelsList.split(","))):
              # late level including subband adjustment
              newlateLevelsList.append(float(lateLevelsList.split(",")[lateBand]) * dB2lin(self.lateLevelAdjust) * dB2lin(self.lateSubbandLevelAdjust[lateBand]))
              
              # late decay
              if not self.lateDecayAdjust==0:
                newlateDecaysList.append(float(lateDecaysList.split(",")[lateBand]) * 1 / self.lateDecayAdjust)
              else:
                print( "late decay adjust value must not be zero" )
                
              # late delay
              currDelay=float(lateDelaysList.split(",")[lateBand])
              newDelay=currDelay+self.lateDelayAdjust
              minDelay=float(obj['room']['lreverb']['delay'])+0.001 #ensure at least 1~ms of late onset ramp
              newlateDelaysList.append(max(minDelay,newDelay))
                
          else:
            print( "Subband level adjustment must be specified in 9 subbands" )
          
          obj['room']['lreverb']['level'] = str(newlateLevelsList)[1:-1]
          obj['room']['lreverb']['decayconst'] = str(newlateDecaysList)[1:-1]
          obj['room']['lreverb']['attacktime'] = str(newlateDelaysList)[1:-1]
  

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
    # OSC example: /EditReverb/editlatelevel 0.0
    if key == "editlatelevel":
      self.lateLevelAdjust = float(valueList[0])
    elif key == "editlatesubbandlevels":
      self.lateSubbandLevelAdjust = valueList[0:]
      print( self.lateSubbandLevelAdjust )
    elif key == "editlatedelay":
      self.lateDelayAdjust = float(valueList[0])
    elif key == "editpredelay":
      self.preDelayAdjust = float(valueList[0])
      print( self.preDelayAdjust )
      if self.preDelayAdjust<0:
        print( "predelay must not be less than zero" )
    elif key == "editpredelaylate":
      self.preDelayAdjustLate = float(valueList[0])
      print( self.preDelayAdjustLate )
      if self.preDelayAdjustLate<0:
        print( "predelay must not be less than zero" )
    elif key == "editearlylevel":
      self.earlyLevelAdjust = float(valueList[0])
    elif key == "editlatedecay":
      self.lateDecayAdjust = float(valueList[0])
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
      raise KeyError("EditReverbProcessor supports keys editearlylevel, editlatelevel, editlatedecay")
