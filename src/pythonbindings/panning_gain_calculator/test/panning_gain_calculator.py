import os

os.chdir("C:/Users/andi/dev/visr-build/python/Debug" )

from panning_gain_calculator import PanningGainCalculator

s = PanningGainCalculator("C:\\Users\\andi\\dev\\visr\\config\\isvr\\22.1_audiolab.xml")

s.numberOfSources()

s.numberOfLoudspeakers()

v = s.calculateGains( 1.0, 0, 0 )

quit()
