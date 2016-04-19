import os
import sys

configFile = "/Users/af5u13/dev/visr/config/isvr/audiolab_22.1.xml"

pythonObjDir = "/Users/af5u13/dev/visr-build/python/Debug/"

sys.path.append( pythonObjDir )

from panning_gain_calculator import PanningGainCalculator
import panning_gain_calculator as vbap

s = PanningGainCalculator( configFile )

s.numberOfSources()

s.numberOfLoudspeakers()

v = s.calculateGains( 1.0, 0, 0 )

quit()
