
import visr
import panningdsp
import pml
import rrl

import numpy as np
import matplotlib.pyplot as plt


def test_initialiseDefault():
    numObjects = 4
    numLoudspeakers = 8
    alignment = 16

    pmg1 = panningdsp.PanningMatrixParameter(numObjects,
                                             numLoudspeakers,
                                             alignment)

    assert pmg1.numberOfObjects == numObjects
    assert pmg1.numberOfLoudspeakers == numLoudspeakers

# Enable to run the unit test as a script.
if __name__ == "__main__":
    test_initialiseDefault()
