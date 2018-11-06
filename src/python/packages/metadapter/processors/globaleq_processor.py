# -*- coding: utf-8 -*-
"""
Created on Thu Mar 16 2017

@author: Jon Francombe
"""

#import numpy

from metadapter import SequenceProcessorInterface

class GlobalEQProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)
        
        # Processor properties
        # Get arguments:
        if 'on' in arguments.attrib:
            self.on = int(arguments.attrib['on'])
        else:
            self.on = 1   # Default is on
        
        if 'verbose' in arguments.attrib:
            self.verbose = int(arguments.attrib['verbose'])
        else:
            self.verbose = 0   # Default is not verbose       
       
        
        # EQ properties
        self.lowgain = 0
        self.highgain = 0
        
        if 'lowfreq' in arguments.attrib:
            self.lowfreq = float(arguments.attrib['lowfreq'])
        else:
            self.lowfreq = 200.0   # Default is 200 Hz
        
        if 'highfreq' in arguments.attrib:
            self.highfreq = float(arguments.attrib['highfreq'])
        else:
            self.highfreq = 2000.0   # Default is 2000 Hz
        
        
        #leveldB = float(arguments.attrib['volumeAdjust'])
        #self.volumeChange = numpy.power( 10.0, leveldB/20.0 )

    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.
        
        if self.on:
        
            eq_low = {u'type': u'lowshelf', u'f': float(self.lowfreq), u'q': 1.0, u'gain': float(self.lowgain)}
            eq_high = {u'type': u'highshelf', u'f': float(self.highfreq), u'q': 1.0, u'gain': float(self.highgain)}

            for obj in objectVector:
                obj[u'eq'] = [eq_low, eq_high]
                
        
        return objectVector


    
    def setParameter( self, key, valueList ):

        onstatus = ['off','on']
        
        if key == "lowgain":
            self.lowgain = valueList[0]
            if self.verbose and self.on:
                print( "Low gain set at %2.2f dB" % (self.lowgain) )

        elif key == "highgain":
            self.highgain = valueList[0]
            if self.verbose and self.on:
                print( "High gain set at %2.2f dB" % (self.highgain) )

        elif key == "lowfreq":
            self.lowfreq = valueList[0]
            if self.verbose and self.on:
                print( "Low frequency set at %2.2f Hz" % (self.lowfreq) )

        elif key == "highfreq":
            self.highfreq = valueList[0]
            if self.verbose and self.on:
                print( "High frequency set at %2.2f Hz" % (self.highfreq) )

        elif key == "on":
            if valueList[0] == 0 or valueList[0] == 1:
                self.on = valueList[0]
                if self.verbose:
                    print("GlobalEQ processor is %s" % onstatus[self.on])
            else:
                raise KeyError( "Command \"on\" must be 0 or 1" )

        else:
            raise KeyError( "Envelopment processor supports only the parameter set commands \"on\" and \"envelopment\"")
