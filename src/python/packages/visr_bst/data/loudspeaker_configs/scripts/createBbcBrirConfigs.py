# -*- coding: utf-8 -*-
"""
Copyright Andreas Franck 2018 - All rights reserved
Copyright Institute of Sound and Vibration Research, University od Southampton - All rights reserved

Created on Thu May  3 08:18:48 2018

@author: Andreas Franck
"""

from loudspeakerconfig import createArrayConfigFromSofa
from configFromSofa import configFromSofa

systemAlabels = [ 'M+030', 'M-030' ]
systemBlabels = systemAlabels + [ 'M+000', 'M+110', 'M-110' ]
systemClabels = systemBlabels + [ 'U+030', 'U-030' ]
systemDlabels = systemClabels + [ 'U+110', 'U-110' ]
systemElabels = systemDlabels + [ 'B+000' ]
systemFlabels =  [ 'M+000', 'M+030', 'M-030', 'U+045', 'U-045', 'M+090', 'M-090', 'M+135', 'M-135', 'U+180' ]
systemGlabels =  [ 'M+000', 'M+045', 'M-045', 'M+030', 'M-030', 'M+090', 'M-090', 'M+135', 'M-135',
                   'U+045', 'U-045', 'U+110', 'U-110' ]
systemHlabels =  [ 'M+060', 'M-060',
                   'M+000',
                   'M+135', 'M-135',
                   'M+030', 'M-030',
                   'M+180',
                   'M+090', 'M-090',
                   'U+045', 'U-045', 'U+000',
                   'T+000',
                   'U+135', 'U-135',
                   'U+090', 'U-090',
                   'U+180',
                   'B+000', 'B+045', 'B-045'
                  ]
allSpeakerLabels = ['B+045', 'B+000', 'B-045', 'B-135', 'B+135',
                    'M+045', 'M+030', 'M+000', 'M-030', 'M-045',
                    'M-060', 'M-090', 'M-110', 'M-135', 'M+180',
                    'M+135', 'M+110', 'M+090', 'M+060',
                    'U+045', 'U+030', 'U+000', 'U-030', 'U-045', 'U-090', 'U-110',
                    'U-135', 'U+180', 'U+135', 'U+110', 'U+090',
                    'T+000' ]

createArrayConfigFromSofa( '../bbcrdlr_systemA.sofa', '../bbcrdlr_systemA.xml', systemAlabels, twoDSetup=True,
               virtualLoudspeakers = [ {'id': 'VotD', 'pos': [-1.0, 0.0 ], "routing": [ ("M+030", 0.5), ("M-030", 0.5) ] } ] )
createArrayConfigFromSofa( '../bbcrdlr_systemB.sofa', '../bbcrdlr_systemB.xml', systemBlabels, twoDSetup=True )
createArrayConfigFromSofa( '../bbcrdlr_systemC.sofa', '../bbcrdlr_systemC.xml', systemClabels,
               virtualLoudspeakers = [ {'id': 'VotD', 'pos': [0.0,0.0,-1.0], "routing": [ ("M+030", 0.2), ("M-030", 0.2), ("M+000", 0.2), ("M+110", 0.2), ("M-110", 0.2) ]} ] )
createArrayConfigFromSofa( '../bbcrdlr_systemD.sofa', '../bbcrdlr_systemD.xml', systemDlabels,
               virtualLoudspeakers = [ {'id': 'VotD', 'pos': [0.0,0.0,-1.0], "routing": [ ("M+030", 0.2), ("M-030", 0.2), ("M+000", 0.2), ("M+110", 0.2), ("M-110", 0.2) ]} ] )
createArrayConfigFromSofa( '../bbcrdlr_systemE.sofa', '../bbcrdlr_systemE.xml', systemElabels,
               virtualLoudspeakers = [ {'id': 'VotD', 'pos': [0.0,0.0,-1.0], "routing": [ ("M+030", 0.2), ("M-030", 0.2), ("B+000", 0.2), ("M+110", 0.2), ("M-110", 0.2) ]} ] )
createArrayConfigFromSofa( '../bbcrdlr_systemF.sofa', '../bbcrdlr_systemF.xml', systemFlabels,
               virtualLoudspeakers = [ {'id': 'VotD', 'pos': [0.0,0.0,-1.0], "routing": [ ("M+030", 0.14286), ("M-030", 0.14286), ("M+000", 0.14286), ("M+090", 0.14286),
                 ("M-090", 0.14286), ("M+135", 0.14286), ("M-135", 0.14286) ]} ] )
createArrayConfigFromSofa( '../bbcrdlr_systemG.sofa', '../bbcrdlr_systemG.xml', systemGlabels,
               virtualLoudspeakers = [ {'id': 'VotD', 'pos': [0.0,0.0,-1.0],
                 "routing": [ ("M+000", 0.1111111), ("M+045", 0.1111111), ("M-045", 0.1111111), ("M+030", 0.1111111),
                              ("M-030", 0.1111111), ("M+090", 0.1111111), ("M-090", 0.1111111), ("M+135", 0.1111111),
                              ("M-135", 0.1111111) ]} ] )
createArrayConfigFromSofa( '../bbcrdlr_systemH.sofa', '../bbcrdlr_systemH.xml', systemHlabels,
               virtualLoudspeakers = [ {'id': 'VotD', 'pos': [0.0,0.0,-1.0],
                "routing": [ ("B+000", 0.1), ("B+045", 0.1), ("B-045", 0.1), ("M+060", 0.1), ("M-060", 0.1),
                 ("M+090", 0.1), ("M-090", 0.1), ("M+135", 0.1), ("M-135", 0.1), ("M+180", 0.1)]} ] )
createArrayConfigFromSofa( '../bbcrdlr_reduced_onsets.sofa', '../bbcrdlr_all_speakers.xml', allSpeakerLabels  )
