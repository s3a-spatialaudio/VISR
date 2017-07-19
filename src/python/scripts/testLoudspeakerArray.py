# -*- coding: utf-8 -*-
"""
Created on Tue Feb 14 15:59:11 2017

@author: af5u13
"""

#exec(open("/home/andi/dev/visr/src/python/scripts/testPanningCalculator.py").read())


import visr
import panning
import numpy as np;

#conffile = """<?xml version=\"1.0\" encoding=\"utf-8\"?>
#<panningConfiguration dimension=\"2\" infinite=\"false\">
#  <outputEqConfiguration numberOfBiquads=\"1\" type=\"iir\">
#    <filterSpec name=\"lowpass\">
#      <biquad a1=\"-1.9688283\" a2=\"0.96907117\" b0=\"6.0729856e-05\" b1=\"0.00012145971\" b2=\"6.0729856e-05\"/>
#    </filterSpec>
#    <filterSpec name=\"highpass\">
#      <biquad a1=\"-1.9688283\" a2=\"0.96907117\" b0=\"-0.98447486\" b1=\"1.9689497\" b2=\"-0.98447486\"/>
#    </filterSpec>
#  </outputEqConfiguration>
#  <loudspeaker channel=\"2\" delay=\"0\" eq=\"highpass\" gainDB=\"0\" id=\"T-030\">
#    <cart x=\"2.17\" y=\"1.36\" z=\"0\"/>
#  </loudspeaker>
#  <loudspeaker channel=\"1\" delay=\"9.8764e-05\" eq=\"highpass\" gainDB=\"-0.35712\" id=\"T+030\">
#    <cart x=\"2.15\" y=\"-1.22\" z=\"0.01\"/>
#  </loudspeaker>
#  <virtualspeaker eq=\"highpass\" id=\"3\">
#    <cart x=\"-1\" y=\"0\" z=\"0\"/>
#    <route lspId=\"T-030\" gainDB=\"0\"/>
#    <route lspId=\"T+030\" gainDB=\"0\"/>
#  </virtualspeaker>
#  <subwoofer assignedLoudspeakers=\"T-030, T+030\" channel=\"3\" delay=\"0\" eq=\"lowpass\" gainDB=\"0\" weights=\"1.000000, 1.000000\"/>
#  <triplet l1=\"T+030\" l2=\"3\"/>
#  <triplet l1=\"T-030\" l2=\"T+030\"/>
#  <triplet l1=\"3\" l2=\"T-030\"/>
#</panningConfiguration>"""

filename = 'C:/Local/gc1y17/visr/config/isvr/audiolab_stereo_1sub_with_rerouting.xml'
conffile = open(filename,'r')
lc = panning.LoudspeakerArray(filename)
#lc.loadXmlString(conffile)
print("FILE", conffile.read())

numRegSpeakers = lc.numberOfRegularLoudspeakers
numTotSpeakers = lc.totNumberOfLoudspeakers
numSub = lc.numberOfSubwoofers
numTrip = lc.numberOfTriplets
is2D = lc.is2D
print( '\n','GENERAL CONFIGURATION ')
print('Number of regular speakers: ', numRegSpeakers)
print('Number of virtual speakers: ', numTotSpeakers-numRegSpeakers)
print('Total number speakers (virtual included): ',numTotSpeakers)
print('Number of subwoofers: ', numSub)
print('Number of triplets: ', numTrip)
print('2D layout: ',is2D)
print('Infinite distance: ', lc.isInfinite)
 
print( '\n','LIST OF LOUDSPEAKER CHANNELS')    
print('Loudspeaker channel indices: ', lc.channelIndices())    
print('Detail:')
for idx in range(0,numRegSpeakers):
    
    # THE METHODS getSpeakerChannel and getSpeakerChannelIndex are redundant in the binding
    
    #print('Channel of loudspeaker at index ',idx,': ',lc.getSpeakerChannel(idx))
    print('Channel index of loudspeaker at index ',idx,': ',lc.getSpeakerChannelIndex(idx))


print( '\n','LIST OF SUBWOOFER CHANNELS')    
print('Subwoofer indices: ',lc.subwooferChannelIndices())
print('Detail:')
for idx in range(0,numSub):
    print('Channel index of subwoofer at index ',idx,': ',lc.subwooferChannelIndex(idx))

print( '\n','LIST OF TRIPLETS')    
for idx in range(0,numTrip):
    if is2D:
        print('Triplet at index ',idx,': ',lc.getTriplet(idx)[0:2])
    else:
        print('Triplet at index ',idx,': ',lc.getTriplet(idx))
