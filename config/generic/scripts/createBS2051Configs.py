# Copyright (c) 2015, Andreas Franck,
# Institute of Sound and Vibration Research,
# University of Southampton, United Kingdom
# a.franck@soton.ac.uk

# Script to generate generic BS-2051 loudspeaker configurations.

# Ensure that $VISR/src/libpanning/test/matlab is in the path.
# TODO: Add more configurations as required
# TODO: Add subwoofer configs where necessary.
# TODO: Add support for variable output channel indices.

import numpy as np

from loudspeakerconfig import createArrayConfigFile, deg2rad, sph2cart


allSpeakerCoordsSpherical = np.asarray( [
[ 0, 0],
[ -22.5, 0],
[ +22.5, 0],
[ -15, 0],
[ +15, 0],
[ -30, 0],
[ +30, 0],
[ -45, 0],
[ +45, 0],
[ -60, 0],
[ +60, 0],
[ -90, 0],
[ +90, 0],
[ -110, 0],
[ +110, 0],
[ -135, 0],
[ +135, 0],
[ +180, 0],
[ 0, +30],
[ -22.5, +30],
[ +22.5, +30],
[ -30, +30],
[ +30, +30],
[ -45, +30],
[ +45, +30],
[ -60, +30],
[ +60, +30],
[ -90, +30],
[ +90, +30],
[ -110, +30],
[ +110, +30],
[ -135, +30],
[ +135, +30],
[ +180, +30],
[ 0, +90],
[   0, -30],
[  -22.5, -30],
[ +22.5, -30],
[  -30, -30],
[  +30, -30],
[  -45, -30],
[  +45, -30],
[  -60, -30],
[  +60, -30],
[  -90, -30],
[  +90, -30],
[ -110, -30],
[  110, -30],
[ -135, -30],
[  135, -30],
[ +180, -30],
[  -45, -30],
[  +45, -30],
] )

allSpeakerLabels = [
'M+000', #  1
'M-022', #  2
'M+022', #  3
'M-SC',  #  4
'M+SC',  #  5
'M-030', #  6
'M+030', #  7
'M-045', #  8
'M+045', #  9
'M-060', # 10
'M+060', # 11
'M-090', # 12
'M+090', # 13
'M-110', # 14
'M+110', # 15
'M-135', # 16
'M+135', # 17
'M+180', # 18
'U+000', # 19
'U-022', # 20
'U+022', # 21
'U-030', # 22
'U+030', # 23
'U-045', # 24
'U+045', # 25
'U-060', # 26
'U+060', # 27
'U-090', # 28
'U+090', # 29
'U-110', # 30
'U+110', # 31
'U-135', # 32
'U+135', # 33
'U+180', # 34
'T+000', # 35
'B+000', # 36
'B-022', # 37
'B+022', # 38
'B-030', # 39
'B+030', # 40
'B-045', # 41
'B+045', # 42
'B-060', # 43
'B+060', # 44
'B-090', # 45
'B+090', # 46
'B-110', # 47
'B+110', # 48
'B-135', # 49
'B+135', # 50
'B+180'  # 51
# 'LFE1',  # 52
#'LFE2',  # 53
]

numAllSpeakers = allSpeakerCoordsSpherical.shape[0]
allSpeakersCart = sph2cart( deg2rad(allSpeakerCoordsSpherical[:,0]),
                     deg2rad(allSpeakerCoordsSpherical[:,1]), 1).T

# System A
bs2051_0_2_0_plain = { "name": 'bs2051-0+2+0-rear-fading',
  "comment": "Defined in ITU-R BS-2051 as system A (stereo), energy-discarding virtual loudspeaker at the rear, no subwoofers.",
  "speakers": ["M+030", "M-030"],
  "channels": [1, 2 ],
  "twoDconfig": True,
  "virtualSpeakers": [ { "id": "rear", "pos": [-1.0, 0.0] }]
}

bs2051_0_2_0_downmix = { "name": 'bs2051-0+2+0-no-subwoofer',
  "comment": "Defined in ITU-R BS-2051 as system A (stereo), virtual loudspeaker at the rear routed to the left and right speaker, no subwoofers.",
  "speakers": ["M+030", "M-030"],
  "channels": [1, 2 ],
  "twoDconfig": True,
  "virtualSpeakers": [ { "id": "rear", "pos": [-1.0, 0.0], "routing": [ ("M+030", 0.5), ("M-030", 0.5) ] }]
}

bs2051_0_2_0_subwoofer = { "name": 'bs2051-0+2+0',
  "comment": "Defined in ITU-R BS-2051 as system A (stereo), virtual loudspeaker at the rear routed to the left and right speaker, one subwoofer.",
  "speakers": ["M+030", "M-030"],
  "channels": [1, 2 ],
  "twoDconfig": True,
  "virtualSpeakers": [ { "id": "rear", "pos": [-1.0, 0.0], "routing": [ ("M+030", 0.5), ("M-030", 0.5) ] }],
  "subwooferConfig": [ {"name": "LFE", "assignedSpeakers": ["M+030", "M-030"],
                        "channel": 3 }]
}

bs2051_0_5_0_plain = { "name": 'bs2051-0+5+0-no-subwoofer',
  "comment": "Defined in ITU-R BS-2051 as system B (5.0), no subwoofer.",
  "speakers": ["M+030", "M-030", "M+000", "M+110", "M-110" ],
  "channels": [1, 2, 3, 5, 6],
  "twoDconfig": True
}

bs2051_0_5_0_subwoofer = { "name": 'bs2051-0+5+0',
  "comment": "Defined in ITU-R BS-2051 as system B (5.1), one subwoofer.",
  "speakers": ["M+030", "M-030", "M+000", "M+110", "M-110" ],
  "channels": [1, 2, 3, 5, 6],
  "twoDconfig": True,
  "subwooferConfig": [ {"name": "LFE", "assignedSpeakers": ["M+030", "M-030", "M+000", "M+110", "M-110"],
                        "channel": 4 }]
}

bs2051_2_5_0_plain = { "name": 'bs2051-2+5+0-no-subwoofer',
  "comment": "Defined in ITU-R BS-2051 as system C, no subwoofers",
  "speakers": ["M+030", "M-030", "M+000", "M+110", "M-110", "U+030", "U-030"  ],
  "channels": [1, 2, 3, 5, 6, 7, 8],
  "virtualSpeakers": [ { "id": "VoG", "pos": [0.0, 0.0,1.0],
    "routing": [ ("U+030", 0.25), ("U-030", 0.25), ("M+110", 0.25), ("M-110", 0.25) ] },
                       { "id": "VotD", "pos": [0.0, 0.0,-1.0],
    "routing": [ ("M+030", 0.2), ("M-030", 0.2), ("M+000", 0.2), ("M+110", 0.2), ("M-110", 0.2) ] }],
  "subwooferConfig": [ {"name": "LFE", "assignedSpeakers": ["M+030", "M-030", "M+000", "M+110", "M-110", "U+030", "U-030" ],
                        "channel": 4 } ]
}

bs2051_2_5_0_subwoofer = { "name": 'bs2051-2+5+0',
  "comment": "Defined in ITU-R BS-2051 as system C, one subwoofer",
  "speakers": ["M+030", "M-030", "M+000", "M+110", "M-110", "U+030", "U-030"  ],
  "channels": list(range(1,8)),
  "virtualSpeakers": [ { "id": "VoG", "pos": [0.0, 0.0,1.0],
                         "routing": [ ("U+030", 0.25), ("U-030", 0.25), ("M+110", 0.25), ("M-110", 0.25) ] },
                       { "id": "VotD", "pos": [0.0, 0.0,-1.0],
                         "routing": [ ("M+030", 0.2), ("M-030", 0.2), ("M+000", 0.2), ("M+110", 0.2), ("M-110", 0.2) ] }]
}

bs2051_4_5_0_plain = { "name": 'bs2051-4+5+0-no-subwoofer',
  "comment": "Defined in ITU-R BS-2051 as system D, no subwoofer",
  "speakers": ["M+030", "M-030", "M+000", "M+110", "M-110", "U+030", "U-030", "U+110", "U-110"  ],
  "channels": [1, 2, 3, 5, 6, 7, 8, 9, 10],
  "virtualSpeakers": [ { "id": "VoG", "pos": [0.0, 0.0,1.0],
                         "routing": [ ("U+030", 0.25), ("U-030", 0.25), ("U+110", 0.25), ("U-110", 0.25) ] },
                       { "id": "VotD", "pos": [0.0, 0.0,-1.0],
                         "routing": [ ("M+030", 0.2), ("M-030", 0.2), ("M+000", 0.2), ("M+110", 0.2), ("M-110", 0.2) ] }]
}

bs2051_4_5_0_subwoofer = { "name": 'bs2051-4+5+0',
  "comment": "Defined in ITU-R BS-2051 as system D, one subwoofer",
  "speakers": ["M+030", "M-030", "M+000", "M+110", "M-110", "U+030", "U-030", "U+110", "U-110"  ],
  "channels": [1, 2, 3, 5, 6, 7, 8, 9, 10],
  "virtualSpeakers": [ { "id": "VoG", "pos": [0.0, 0.0,1.0],
                         "routing": [ ("U+030", 0.25), ("U-030", 0.25), ("U+110", 0.25), ("U-110", 0.25) ] },
                       { "id": "VotD", "pos": [0.0, 0.0,-1.0],
                         "routing": [ ("M+030", 0.2), ("M-030", 0.2), ("M+000", 0.2), ("M+110", 0.2), ("M-110", 0.2) ] }],
  "subwooferConfig": [ {"name": "LFE", "assignedSpeakers": ["M+030", "M-030", "M+000", "M+110", "M-110", "U+030", "U-030", "U+110", "U-110" ],
                        "channel": 4 } ]
}

bs2051_4_5_1_plain = { "name": 'bs2051-4+5+1-no-subwoofer',
  "comment": "Defined in ITU-R BS-2051 as system D, one subwoofer",
  "speakers": ["M+030", "M-030", "M+000", "M+110", "M-110", "U+030", "U-030", "U+110", "U-110", "B+000"  ],
  "channels": [1, 2, 3, 5, 6, 7, 8, 9, 10, 11 ],
  "virtualSpeakers": [ { "id": "VoG", "pos": [0.0, 0.0,1.0],
                         "routing": [ ("U+030", 0.25), ("U-030", 0.25), ("U+110", 0.25), ("U-110", 0.25) ] },
                       { "id": "VotD", "pos": [0.0, 0.0,-1.0],
                         "routing": [ ("M+030", 0.2), ("M-030", 0.2), ("M+000", 0.2), ("M+110", 0.2), ("M-110", 0.2) ] }]
}


bs2051_4_5_1_subwoofer = { "name": 'bs2051-4+5+1',
  "comment": "Defined in ITU-R BS-2051 as system D, one subwoofer",
  "speakers": ["M+030", "M-030", "M+000", "M+110", "M-110", "U+030", "U-030", "U+110", "U-110", "B+000"  ],
  "channels": [1, 2, 3, 5, 6, 7, 8, 9, 10, 11 ],
  "virtualSpeakers": [ { "id": "VoG", "pos": [0.0, 0.0,1.0],
                         "routing": [ ("U+030", 0.25), ("U-030", 0.25), ("U+110", 0.25), ("U-110", 0.25) ] },
                       { "id": "VotD", "pos": [0.0, 0.0,-1.0],
                         "routing": [ ("M+030", 0.2), ("M-030", 0.2), ("M+000", 0.2), ("M+110", 0.2), ("M-110", 0.2) ] }],
  "subwooferConfig": [ {"name": "LFE", "assignedSpeakers": ["M+030", "M-030", "M+000", "M+110", "M-110",
                                                            "U+030", "U-030", "U+110", "U-110", "B+000" ],
                        "channel": 4 } ]
}


bs2051_3_7_0_plain = { "name": 'bs2051-3+7+0-no-subwoofer',
  "comment": "Defined in ITU-R BS-2051 as system F, no subwoofers",
  "speakers": ["M+000", "M+030", "M-030", "U+045", "U-045", "M+090", "M-090", "M+135", "M-135", "U+180" ],
  "channels": list(range(1,11)),
  "virtualSpeakers": [  { "id": "VoG", "pos": [0.0, 0.0,1.0],
                         "routing": [ ("U+045", 1.0/3), ("U-045", 1.0/3), ("U+180", 1.0/3) ] },
                        { "id": "VotD", "pos": [0.0, 0.0,-1.0],
                          "routing": [ ("M+030", 0.14286), ("M-030", 0.14286), ("M+000", 0.14286), ("M+090", 0.14286),
                                       ("M-090", 0.14286), ("M+135", 0.14286), ("M-135", 0.14286) ] }]
}


bs2051_3_7_0_subwoofers = { "name": 'bs2051-3+7+0',
  "comment": "Defined in ITU-R BS-2051 as system F, two subwoofers",
  "speakers": ["M+000", "M+030", "M-030", "U+045", "U-045", "M+090", "M-090", "M+135", "M-135", "U+180" ],
  "channels": list(range(1,11)), # 1..10
  "virtualSpeakers": [  { "id": "VoG", "pos": [0.0, 0.0,1.0],
                         "routing": [ ("U+045", 1.0/3), ("U-045", 1.0/3), ("U+180", 1.0/3) ] },
                        { "id": "VotD", "pos": [0.0, 0.0,-1.0],
                          "routing": [ ("M+030", 0.14286), ("M-030", 0.14286), ("M+000", 0.14286), ("M+090", 0.14286),
                                       ("M-090", 0.14286), ("M+135", 0.14286), ("M-135", 0.14286) ] }],
  "subwooferConfig": [ {"name": "LFE1",
                        "assignedSpeakers": ["M+000", "M+030", "U+045", "M+090", "M+135", "U+180" ],
                        "weights": [ 0.5, 1.0, 1.0, 1.0, 1.0, 0.5 ],
                        "channel": 11 },
                       {"name": "LFE1",
                        "assignedSpeakers": ["M+000", "M+030", "U+045", "M+090", "M+135", "U+180" ],
                        "weights": [ 0.5, 1.0, 1.0, 1.0, 1.0, 0.5 ],
                        "channel": 12 }]
}

bs2051_4_9_0_plain = { "name": 'bs2051-4+9+0-no-subwoofer',
  "comment": "Defined in ITU-R BS-2051 as system G, no subwoofer",
  "speakers": [ 'M+000', 'M+045', 'M-045', 'M+030', 'M-030', 'M+090', 'M-090', 'M+135', 'M-135',
                'U+045', 'U-045', 'U+110', 'U-110' ],
  "virtualSpeakers": [ { "id": "VoG", "pos": [0.0, 0.0,1.0],
                         "routing": [ ("U+045", 0.25), ("U-045", 0.25), ("U+110", 0.25), ("U-110", 0.25) ] },
                       { "id": "VotD", "pos": [0.0, 0.0,-1.0],
    "routing": [ ("M+000", 0.1111111), ("M+045", 0.1111111), ("M-045", 0.1111111), ("M+030", 0.1111111),
                 ("M-030", 0.1111111), ("M+090", 0.1111111), ("M-090", 0.1111111), ("M+135", 0.1111111),
                 ("M-135", 0.1111111) ] }],
  "channels": list(range(1,14))
}

bs2051_4_9_0_subwoofer = { "name": 'bs2051-4+9+0',
  "comment": "Defined in ITU-R BS-2051 as system G, one subwoofer",
  "speakers": [ 'M+000', 'M+045', 'M-045', 'M+030', 'M-030', 'M+090', 'M-090', 'M+135', 'M-135',
                'U+045', 'U-045', 'U+110', 'U-110' ],
  "virtualSpeakers": [  { "id": "VoG", "pos": [0.0, 0.0,1.0],
                         "routing": [ ("U+045", 0.25), ("U-045", 0.25), ("U+110", 0.25), ("U-110", 0.25) ] },
                        { "id": "VotD", "pos": [0.0, 0.0,-1.0],
    "routing": [ ("M+000", 0.1111111), ("M+045", 0.1111111), ("M-045", 0.1111111), ("M+030", 0.1111111),
                 ("M-030", 0.1111111), ("M+090", 0.1111111), ("M-090", 0.1111111), ("M+135", 0.1111111),
                 ("M-135", 0.1111111) ] }],
  "channels": list(range(1,14)),
  "subwooferConfig": [ {"name": "LFE", "assignedSpeakers": ['M+000', 'M+045', 'M-045', 'M+030', 'M-030', 'M+090', 'M-090', 'M+135', 'M-135',
                'U+045', 'U-045', 'U+110', 'U-110'],
                        "channel": 14 }]
}

bs2051_9_10_3_plain = { "name": 'bs2051-9+10+3-no-subwoofers',
  "comment": "Defined in ITU-R BS-2051 as system H, no subwoofers",
  "speakers": [ 'M+060', 'M-060', 'M+000', 'M+135', 'M-135',
                'M+030', 'M-030', 'M+180', 'M+090', 'M-090',
                'U+045', 'U-045', 'U+000', 'T+000', 'U+135', 'U-135',
                'U+090', 'U-090', 'U+180',
                'B+000', 'B+045', 'B-045' ],
  "virtualSpeakers": [ { "id": "VotD", "pos": [0.0, 0.0,-1.0],
    "routing": [ ("B+000", 0.1), ("B+045", 0.1), ("B-045", 0.1), ("M+060", 0.1), ("M-060", 0.1),
                 ("M+090", 0.1), ("M-090", 0.1), ("M+135", 0.1), ("M-135", 0.1), ("M+180", 0.1)] }],
  "channels": list(range(1,4))+list(range(5,10)) + list(range(11,25)),
}

bs2051_9_10_3_subwoofers = { "name": 'bs2051-9+10+3',
  "comment": "Defined in ITU-R BS-2051 as system H, 2 subwoofers",
  "speakers": [ 'M+060', 'M-060', 'M+000', 'M+135', 'M-135',
                'M+030', 'M-030', 'M+180', 'M+090', 'M-090',
                'U+045', 'U-045', 'U+000', 'T+000', 'U+135', 'U-135',
                'U+090', 'U-090', 'U+180',
                'B+000', 'B+045', 'B-045' ],
  "virtualSpeakers": [ { "id": "VotD", "pos": [0.0, 0.0,-1.0],
    "routing": [ ("B+000", 0.1), ("B+045", 0.1), ("B-045", 0.1), ("M+060", 0.1), ("M-060", 0.1),
                 ("M+090", 0.1), ("M-090", 0.1), ("M+135", 0.1), ("M-135", 0.1), ("M+180", 0.1)] }],

  "channels": list(range(1,4))+list(range(5,10)) + list(range(11,25)),
  "subwooferConfig": [ {"name": "LFE1", "assignedSpeakers": [ 'M+060', 'M+000', 'M+135', 'M+030', 'M+180', 'M+090',
                                                             'U+045', 'U+000', 'T+000', 'U+135', 'U+090', 'U+180',
                                                             'B+000', 'B+045' ],
                        "weigths": [ 1.0, 0.5, 1.0, 1.0, 1.0, 1.0,
                                     1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5, 1.0 ],
                        "channel": 4},
                        {"name": "LFE2", "assignedSpeakers": [ 'M-060', 'M+000', 'M-135', 'M-030', 'M+180', 'M-090',
                                                             'U-045', 'U+000', 'T+000', 'U-135', 'U-090', 'U+180',
                                                             'B+000', 'B-045' ],
                        "weigths": [ 1.0, 0.5, 1.0, 1.0, 1.0, 1.0,
                                     1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5, 1.0 ],
                        "channel": 10 }]
}


bs2051_9_10_3_subwoofers_xover = { "name": 'bs2051-9+10+3-subwoofers-xover',
  "comment": "Defined in ITU-R BS-2051 as system H, two subwoofers and integrated crossover.",
  "speakers": [ 'M+060', 'M-060', 'M+000', 'M+135', 'M-135',
                'M+030', 'M-030', 'M+180', 'M+090', 'M-090',
                'U+045', 'U-045', 'U+000', 'T+000', 'U+135', 'U-135',
                'U+090', 'U-090', 'U+180',
                'B+000', 'B+045', 'B-045' ],
  "virtualSpeakers": [ { "id": "VotD", "pos": [0.0, 0.0,-1.0],
    "routing": [ ("B+000", 0.1), ("B+045", 0.1), ("B-045", 0.1), ("M+060", 0.1), ("M-060", 0.1),
                 ("M+090", 0.1), ("M-090", 0.1), ("M+135", 0.1), ("M-135", 0.1), ("M+180", 0.1)] }],
  "channels": list(range(1,4))+list(range(5,10)) + list(range(11,25)),
  "eqConfiguration": [ {"name": "speakers",
                        "filter": [{ "a": [-1.9688283, 0.96907117],  "b": [-0.98447486, 1.9689497, -0.98447486] }],
                        "loudspeakers": [ 'M+060', 'M-060', 'M+000', 'M+135', 'M-135',
                                          'M+030', 'M-030', 'M+180', 'M+090', 'M-090',
                                          'U+045', 'U-045', 'U+000', 'T+000', 'U+135', 'U-135',
                                          'U+090', 'U-090', 'U+180',
                                          'B+000', 'B+045', 'B-045' ] },
                         {"name": "subwoofer",
                          "filter": [{ "a": [-1.9688283, 0.96907117], "b": [6.0729856e-05, 0.00012145971, 6.0729856e-05] }],
                         }
                     ],
  "subwooferConfig": [ {"name": "LFE1", "assignedSpeakers": [ 'M+060', 'M+000', 'M+135', 'M+030', 'M+180', 'M+090',
                                                             'U+045', 'U+000', 'T+000', 'U+135', 'U+090', 'U+180',
                                                             'B+000', 'B+045' ],
                        "weigths": [ 1.0, 0.5, 1.0, 1.0, 1.0, 1.0,
                                     1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5, 1.0 ],
                        "channel": 4, "eq": "subwoofer" },
                        {"name": "LFE2", "assignedSpeakers": [ 'M-060', 'M+000', 'M-135', 'M-030', 'M+180', 'M-090',
                                                             'U-045', 'U+000', 'T+000', 'U-135', 'U-090', 'U+180',
                                                             'B+000', 'B-045' ],
                        "weigths": [ 1.0, 0.5, 1.0, 1.0, 1.0, 1.0,
                                     1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5, 1.0 ],
                        "channel": 10, "eq": "subwoofer" }]
}


allConfigs = [
bs2051_0_2_0_plain,
bs2051_0_2_0_downmix,
bs2051_0_2_0_subwoofer,
bs2051_0_5_0_plain,
bs2051_0_5_0_subwoofer,
bs2051_2_5_0_plain,
bs2051_2_5_0_subwoofer,
bs2051_4_5_0_plain,
bs2051_4_5_0_subwoofer,
bs2051_4_5_1_plain,
bs2051_4_5_1_subwoofer,
bs2051_3_7_0_plain,
bs2051_3_7_0_subwoofers,
bs2051_4_9_0_plain,
bs2051_4_9_0_subwoofer,
bs2051_9_10_3_plain,
bs2051_9_10_3_subwoofers,
bs2051_9_10_3_subwoofers_xover
]

for cfg in allConfigs:
    speakerLabels = cfg["speakers"]
    speakerIdx = []
    for label in speakerLabels:
        if label not in allSpeakerLabels:
            raise ValueError( "Invalid loudspeaker label '%s'." % label )
        idx = allSpeakerLabels.index( label )
        speakerIdx.append( idx )
    lspPos = allSpeakersCart[ :, speakerIdx ]

    virtualSpeakers = cfg["virtualSpeakers"] if "virtualSpeakers" in cfg else []

    outputFileName = '../' + cfg["name"] + ".xml"

    labels = [allSpeakerLabels[idx] for idx in speakerIdx]

    twoDconfig = cfg["twoDconfig"] if "twoDconfig" in cfg else False

    eqConfig = cfg["eqConfiguration"] if "eqConfiguration" in cfg else None

    subCfg = cfg["subwooferConfig"] if "subwooferConfig" in cfg else []

    comment = cfg["comment"] if "comment" in cfg else None

    createArrayConfigFile( outputFileName = outputFileName,
                           lspPositions = lspPos,
                           twoDconfig = twoDconfig,
                           sphericalPositions=True,
                           channelIndices = cfg["channels"],
                           loudspeakerLabels = labels,
                           virtualLoudspeakers = virtualSpeakers,
                           eqConfiguration = eqConfig,
                           subwooferConfig = subCfg,
                           comment = comment
                           )




