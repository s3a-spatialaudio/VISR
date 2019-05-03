#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Mar 28 15:43:55 2018

@author: af5u13
"""

import graphviz

import visr

def VISRlise( component, recursionLevel = 0 ):
    graph = graphviz.Digraph( component.name )
    
    portLookup = {}

    for comp in component.components():
        audioInputs = [i for i in comp.audioPorts if i.direction == visr.Port.Direction.Input]
        audioOutputs = [i for i in comp.audioPorts if i.direction == visr.Port.Direction.Output]
        paramInputs = [i for i in comp.parameterPorts if i.direction == visr.Port.Direction.Input]
        paramOutputs = [i for i in comp.parameterPorts if i.direction == visr.Port.Direction.Output]
        
        audioInArea = _createPortArea( audioInputs, isVertical=True )
        audioOutArea = _createPortArea( audioOutputs, isVertical=True )
        paramInArea = _createPortArea( paramInputs, isVertical=True )
        paramOutArea = _createPortArea( paramOutputs, isVertical=True )
            
        compLabel = audioInArea + "|{" +  paramInArea + "|" + comp.name +"|" + paramOutArea + "}|"  + audioOutArea
            
        graph.node( comp.name, label=compLabel, shape='record' )
        
    return graph


def _createPortArea( portList, isVertical ):
    if isVertical:
        label = "{"
    else:
        label = ""
    for i,p in enumerate(portList):
        label += "<" + p.name + "> " + p.name
        if i < len(portList)-1:
            label += "|"
    if isVertical:
        label += "}"
    return label
