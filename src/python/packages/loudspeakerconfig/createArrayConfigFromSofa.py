# -*- coding: utf-8 -*-
"""
Created on Thu May  3 08:04:22 2018

@author: af5u13
"""

import numpy as np
import os

from .geometry_functions import deg2rad, sph2cart

from loudspeakerconfig import createArrayConfigFile

def createArrayConfigFromSofa( sofaFile, xmlFile = None, lspLabels = None, twoDSetup = False, virtualLoudspeakers = [] ):
    """
    Create a loudspeaker configuraton file from a SOFA file containing a number of emitters representing loudspeakers.

    Parameters
    ==========

    sofaFile: string
      A file path to a SOFA file.
    xmlFile: string, optional
      Path of the XML output file to be written.
      Optional argument, if not provided, the SOFA file path is used with the extension replaced by ".xml"
    lspLabels: list of strings, optional
      List of loudspeaker labels, must match the number of emitters in the SOFA files.
      If not provided, numbered labels are automatically generated.
    twoDSetup: bool, optional
      Flag specifying whether the aray is to be considered plane (True) or 3D (False).
      Optional value, dafault is False (3D).
    virtualLoudspeakers: list, optional
      A list of virtual loudspeakers to be added to the setup. Each entry must be a Python dict as decribed
      in the function :py:meth:`loudspeakerconfig.createArrayConfigFile`.

    """
    import h5py # Import in the function to avoid a global dependency.

    if not os.path.exists( sofaFile ):
        raise ValueError( "SOFA file does not exist.")

    if xmlFile is None:
        xmlFile = os.path.basename(sofaFile) + '.xml'

    fh = h5py.File( sofaFile )

    ep =fh.get('EmitterPosition')
    emitterCoordSystem = ep.attrs['Type'] # This is a required attribute.
    emitterCoordSystem = emitterCoordSystem.decode("utf-8") # make it a string.

    if emitterCoordSystem == "spherical":
        posSph = np.squeeze( np.asarray(ep) )
        posSph[:,0] = deg2rad( posSph[:,0] )
        posSph[:,1] = deg2rad( posSph[:,1] )
        posCart = sph2cart( posSph[:,0], posSph[:,1], posSph[:,2] )
    else:
        posCart = np.squeeze( np.asarray(ep) )
    if twoDSetup:
        posCart = posCart[:,0:2]

    createArrayConfigFile( xmlFile,
                          posCart.T,
                          loudspeakerLabels = lspLabels,
                          twoDconfig = twoDSetup,
                          sphericalPositions = True,
                          virtualLoudspeakers = virtualLoudspeakers )

    fh.close()

