# -*- coding: utf-8 -*-
"""
Created in 2017

@author: Jon Francombe
"""

import numpy
import math

from metadapter import SequenceProcessorInterface


#class FakeGroupsProcessor(MetadataProcessorInterface):
class FakeGroupsProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        #MetadataProcessorInterface.__init__(self, arguments)
        SequenceProcessorInterface.__init__(self, arguments)
        #leveldB = float(arguments.attrib['volumeAdjust'])
        #self.volumeChange = numpy.power( 10.0, leveldB/20.0 )
        self.programme = 'None'

    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.

        ### GROUPS: ###
        # 1: Clean speech
        # 2: Non-diegetic music
        # 3: Foreground action sounds
        # 4: Background action sounds
        # 5: Reverb and effects

        debug = False

        if self.programme == 'Forest':

            # This version uses the background sounds group:
            groups = [1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4]

            # This version uses the reverb and effects group:
            #groups = [1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5]

        elif self.programme == 'Frame':
            groups = [1,4,4,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,4,4,4,4,4,4]


        elif self.programme == 'None':
            return objectVector

        if debug is True:
            print ("Number of objects: {}".format(len(objectVector)))
            print ("Number of groups specified: {}".format(len(groups)))


        # Add the groups
        for i,obj in enumerate(objectVector):

            if i == 2 and debug is True:
                print (obj)

            # Set the group
            obj['group'] = unicode(str(groups[i]))

            # For testing — write junk into the metadata, see if the renderer complains
            # obj['myattribute'] = u'ThisIsAString'
            # print obj
            # It doesn't seem to complain

            if i == 2 and debug is True:
                print (obj)
                print()

        return objectVector

        
    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""

        if (key != "programme") or (len( valueList ) != 1):
            raise KeyError( "FakeGroupsProcessor supports only the parameter set command \"programme\" consisting of a single string" )


        else: # key is programme
            self.programme = valueList[0]

            print( "FakeGroups: Programme set to: {}".format(self.programme))
