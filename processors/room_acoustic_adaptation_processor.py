# -*- coding: utf-8 -*-
"""
Created on Sat 5 March 20:01 2016

@author: Dylan Menzies
"""

#import numpy
import json
#import lxml
import math

from lxml import objectify

from metadapter import SequenceProcessorInterface

class RoomAcousticAdaptationProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)
#        leveldB = float(arguments.attrib['volumeAdjust'])
#        self.volumeChange = numpy.power( 10.0, leveldB/20.0 )


#       Load Room Array Response (RAR)
        #! Need to launch metadapter.py from inside /metadapter
        fp = open('./processors/room/RoomArrayResponse.json')
        self.RAR = json.load(fp)['objects']
        self.nRAR = len(self.RAR)
        #print(self.nRAR)
        #print(self.RAR[1]['type'])

#       Load Array Config (AC)
        fp = open('./processors/room/ArrayConfig.xml')
        self.AC = objectify.fromstring(fp.read())
        az1 = self.AC.loudspeaker[1].polar.get("az")
        # hasattr(self.AC.loudspeaker[1], 'polar')
        #print(az1)
        r = self.AC.loudspeaker[1].polar.get("r")
        #print(r)



        self.lateBandFreqs = (62.5, 125, 250, 500, 1000, 2000, 4000, 8000, 16000)

#       Init state

        self.sampleRate = 48000


        # Reproduction room parameters

        # Processs RAR here to generate room adaptation parameters:

        self.roomAdaptOn = 0;
        self.roomLateLevel = 0.0;
        self.roomLateDecayConst = 1.0
        self.roomLateFreqMax = self.sampleRate
        self.roomLateDelay = 0.0



        self.directLevelScale = 1.0
        self.earlyLevelScale = 1.0
        self.lateLevelScale = 1.0
        self.lateBandLevelScale = [1.0] *9

        self.earlyDelayScale = 1.0
        self.earlyDelayOffset = 0.0
        self.lateDelayScale = 1.0
        self.lateDelayOffset = 0.0
        self.lateAttackTimeScale = 1.0
        self.lateAttackTimeOffset = 0.0
        self.lateDecayTimeScale = 10.0
        self.lateDecayTimeOffset = 0.0

        I = { 'a0': 1, 'a1': 0, 'a2': 0, 'b0': 1, 'b1': 0, 'b2': 0 }
        self.directLowpassBiquad = I
        self.directHighpassBiquad = I
        self.earlyLowpassBiquad = I
        self.earlyHighpassBiquad = I

        self.lateLowpassCutoff = self.sampleRate
        self.lateHighpassCutoff = 0




    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.
        # print( "ChangeVolumeProcessor::processObjectVector() called." )
	   # print('RECEIVE OBJECT');



        for obj in objectVector:
            # Assuming type 'pointreverb' for now:



            # Automatic late reverberation adjust based on RAR

            if (self.roomAdaptOn):
                ## possibly increase roomLateFreqMax based on LateFreqMax calculated for object:
                roomLateFreqMax = self.roomLateFreqMax

                roomAdaptLateLevelScale = ( math.sqrt(2 * self.roomLateDecayConst * roomLateFreqMax)
                    / (self.roomLateLevel * self.sampleRate) )

                print(roomAdaptLateLevelScale)

                # If room reverb is small compared to direct: direct part will do the job
                if (roomAdaptLateLevelScale > 1.0): roomAdaptLateLevelScale = 1.0
                roomAdaptLateDelayOffset = - self.roomLateDelay
            else:
                roomAdaptLateLevelScale = 1.0
                roomAdaptLateDelayOffset = 0.0



            # Level adjust

            obj['level'] = float(obj['level']) * self.directLevelScale

            for reflect in obj['room']['ereflect']:
                reflect['level'] = float(reflect['level']) * self.earlyLevelScale

            # Late level in filter section



            # Timing adjust

            for reflect in obj['room']['ereflect']:
                reflect['delay'] = float(reflect['delay']) * self.earlyDelayScale
                reflect['delay'] = float(reflect['delay']) + self.earlyDelayOffset

            obj['room']['lreverb']['delay'] = float(obj['room']['lreverb']['delay']) + roomAdaptLateDelayOffset

            obj['room']['lreverb']['delay'] = float(obj['room']['lreverb']['delay']) * self.lateDelayScale
            obj['room']['lreverb']['delay'] = float(obj['room']['lreverb']['delay']) + self.lateDelayOffset

            if (obj['room']['lreverb']['delay'] < 0):
                obj['room']['lreverb']['delay'] = 0 #! Renderer should allow for negative values.



            decayConstStr = obj['room']['lreverb']['decayconst']
            decayConst = [ float(x) for x in decayConstStr.split(',') ]

            for i, x in enumerate(decayConst):
                y = (self.lateDecayTimeScale / x) - (self.lateDecayTimeOffset * self.sampleRate)
                if (y >= 0): decayConst[i] = 1 # rapid decay
                else: decayConst[i] = 1 / y

            obj['room']['lreverb']['decayconst'] = str(decayConst)[1:-1]


            attackTimeStr = obj['room']['lreverb']['attacktime']
            attackTime = [ float(x) for x in attackTimeStr.split(',') ]

            attackTime = [ self.lateAttackTimeScale * x + self.lateAttackTimeOffset  for x in attackTime ]

            obj['room']['lreverb']['attacktime'] = str(attackTime)[1:-1]




            # Filter adjust

            # Direct

            biquads = obj['biquadsos']
            biquads.append(self.directLowpassBiquad)
            biquads.append(self.directHighpassBiquad)

            # Early

            for reflect in obj['room']['ereflect']:
                biquads = reflect['biquadsos']
                biquads.append(self.earlyLowpassBiquad)
                biquads.append(self.earlyHighpassBiquad)

            # Late

            levelStr = obj['room']['lreverb']['level']
            level = [ float(x) for x in levelStr.split(',') ]

            for i in range(0,9):
                f = self.lateBandFreqs[i]

                r = (float)(self.lateLowpassCutoff) / f
                if (r < 1): level[i] = float(level[i]) * r

                if (self.lateHighpassCutoff > 0):
                    r = f / self.lateHighpassCutoff
                    if (r < 1): level[i] = float(level[i]) * r

                level[i] = float(level[i]) * roomAdaptLateLevelScale * self.lateLevelScale * self.lateBandLevelScale[i]

            obj['room']['lreverb']['level'] = str(level)[1:-1]



            # Direction

#           type = obj['type']
#            if (type == 'pointreverb'):
#                az =  obj['position']['az']
#                print( az, self.nRAR )



        return objectVector





    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
#        if (key != "volume") or (len( valueList ) != 1) or (not isinstance(valueList[0], float) ):
#            raise KeyError( "ChangeVolumeProcessor supports only the parameter set command \"volume\" consisting of a single float value" )
#        leveldB = valueList[0]
#        self.volumeChange = numpy.power( 10.0, leveldB/20.0 )

        print(key, valueList)

        def lowpassBiquad(fc):
            fcn = 2*fc / self.sampleRate    # normalised frequency range
            if (fcn >= 1): beta = 0         # jump to bypass
            else: beta = math.exp(-math.pi*fcn)    # * (1-1./ (1+(0.5-fcn)*100) ) # smoothed bypass
            a0 = 1
            a1 = -beta
            b0 = 1-beta
            b1 = 0
            return { 'a0': a0, 'a1': a1, 'a2': 0, 'b0': b0, 'b1': b1, 'b2': 0 }

#            K = math.tan(math.pi*fc/self.sampleRates)
#            if (K >= 1): <bypass>
#            alpha = 1+K
#            a0 = 1
#            a1 = -(1-K)/alpha
#            b0 = K/alpha
#            b1 = K/alpha


        def highpassBiquad(fc):
            fcn = 2*fc / self.sampleRate
            beta = math.exp(-math.pi*fcn)
            a0 = -1
            a1 = beta
            b0 = -(1+beta)/2
            b1 = (1+beta)/2
            return { 'a0': a0, 'a1': a1, 'a2': 0, 'b0': b0, 'b1': b1, 'b2': 0 }

#            K = math.tan(math.pi*fc/self.sampleRate)
#            if (K >= 1): <bypass>
#            alpha = 1+K
#            a0 = 1
#            a1 = -(1-K)/alpha
#            b0 = 1/alpha
#            b1 = -1/alpha



        if (key == "sampleRate"):
            self.sampleRate = valueList[0]


        elif (key == "roomAdaptOn"):
            self.roomAdaptOn = valueList[0]

        elif (key == "roomLateLevel"):
            self.roomLateLevel = valueList[0]

        elif (key == "roomLateDecayConst"):
            self.roomLateDecayConst = valueList[0]

        elif (key == "roomLateFreqMax"):
            self.roomLateFreqMax = valueList[0]

        elif (key == "roomLateDelay"):
            self.roomLateDelay = valueList[0]



        elif (key == "directLevelScale"):
            self.directLevelScale = valueList[0]

        elif (key == "earlyLevelScale"):
            self.earlyLevelScale = valueList[0]

        elif (key == "lateLevelScale"):
            self.lateLevelScale = valueList[0]


        elif (key == "lateBand1LevelScale"):
            self.lateBandLevelScale[0] = valueList[0]

        elif (key == "lateBand2LevelScale"):
            self.lateBandLevelScale[1] = valueList[0]

        elif (key == "lateBand3LevelScale"):
            self.lateBandLevelScale[2] = valueList[0]

        elif (key == "lateBand4LevelScale"):
            self.lateBandLevelScale[3] = valueList[0]

        elif (key == "lateBand5LevelScale"):
            self.lateBandLevelScale[4] = valueList[0]

        elif (key == "lateBand6LevelScale"):
            self.lateBandLevelScale[5] = valueList[0]

        elif (key == "lateBand7LevelScale"):
            self.lateBandLevelScale[6] = valueList[0]

        elif (key == "lateBand8LevelScale"):
            self.lateBandLevelScale[7] = valueList[0]

        elif (key == "lateBand9LevelScale"):
            self.lateBandLevelScale[8] = valueList[0]


        elif (key == "earlyDelayScale"):
            self.earlyDelayScale = valueList[0]

        elif (key == "earlyDelayOffset"):
            self.earlyDelayOffset = valueList[0]

        elif (key == "lateDelayScale"):
            self.lateDelayScale = valueList[0]

        elif (key == "lateDelayOffset"):
            self.lateDelayOffset = valueList[0]

        elif (key == "lateDelayScale"):
            self.lateDelayScale = valueList[0]

        elif (key == "lateDelayOffset"):
            self.lateDelayOffset = valueList[0]

        elif (key == "lateAttackTimeScale"):
            self.lateAttackTimeScale = valueList[0]

        elif (key == "lateAttackTimeOffset"):
            self.lateAttackTimeOffset = valueList[0]

        elif (key == "lateDecayTimeScale"):
            self.lateDecayTimeScale = valueList[0]
            if (self.lateDecayTimeScale == 0): self.lateDecayTimeScale = 0.01

        elif (key == "lateDecayTimeOffset"):
            self.lateDecayTimeOffset = valueList[0]



        elif (key == "directLowpassCutoff"):
            fc = valueList[0]
            self.directLowpassBiquad = lowpassBiquad(fc)

        elif (key == "directHighpassCutoff"):
            fc = valueList[0]
            self.directHighpassBiquad = highpassBiquad(fc)

        elif (key == "earlyLowpassCutoff"):
            fc = valueList[0]
            self.earlyLowpassBiquad = lowpassBiquad(fc)

        elif (key == "earlyHighpassCutoff"):
            fc = valueList[0]
            self.earlyHighpassBiquad = highpassBiquad(fc)

        elif (key == "lateLowpassCutoff"):
            self.lateLowpassCutoff = valueList[0]

        elif (key == "lateHighpassCutoff"):
            self.lateHighpassCutoff = valueList[0]



