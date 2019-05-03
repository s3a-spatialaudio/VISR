# Adapt radius of groups of objects
# Radius not used by VISR renderer for attenuation, so will have a delay effect only
# Originally written for porting scenes to SoundScape Renderer
# Richard Hughes, University of Salford, r.j.hughes@salford.ac.uk

import math

from metadapter import SequenceProcessorInterface

class GroupAdaptRadiusProcessor(SequenceProcessorInterface):

	def __init__(self, arguments ):
		SequenceProcessorInterface.__init__(self, arguments)
		self.objectLookup = {}
		self.adjust = {}
		for groupNode in arguments.iter( 'group' ):
			groupName = groupNode.attrib['name']
			objsStr = groupNode.attrib['objectIds']
			initialAdjust = float(groupNode.attrib['initialAdjust'])
			objIds = [int(s) for s in objsStr.split() if s.isdigit()]
			for objId in objIds:
				self.objectLookup[objId] = groupName
			self.adjust[groupName] = initialAdjust

	def processObjectVector( self, objectVector):
		# Function to be implemented by all derived SequenceProcessor interfaces.
		for obj in objectVector:
			id = int( obj['id'] )
			if id in self.objectLookup:
				group = self.objectLookup[id]
				adjustRadius = self.adjust[group]
				obj_type = '%-5.5s' % obj['type']
				if obj_type == 'plane':
					obj['direction']['refdist'] = float(obj['direction']['refdist'])*adjustRadius
				elif obj_type == 'point':
					if 'x' in obj['position'].keys():
						obj['position']['x'] = float(obj['position']['x'])*adjustRadius
						obj['position']['y'] = float(obj['position']['y'])*adjustRadius
						obj['position']['z'] = float(obj['position']['z'])*adjustRadius
					else:
						obj['position']['radius'] = float(obj['position']['radius'])*adjustRadius
		return objectVector

	def setParameter( self, key, valueList ):
		""" Set the parameter to a given value."""
		if (len( valueList ) != 1):
			raise KeyError( "GroupAdaptRadiusProcessor supports only commands consisting of a single float value" )
		if not key in self.volumes:
			raise KeyError( "GroupAdaptRadiusProcessor: The command name does not correspond to a known group." )
		self.adjust[key] = float(valueList[0])


