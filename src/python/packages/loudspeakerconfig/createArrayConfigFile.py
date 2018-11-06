# -*- coding: utf-8 -*-
"""
Created on Thu Jan 18 09:43:35 2018

@author: af5u13
"""

import lxml.etree as ET

from scipy.spatial import ConvexHull
import numpy as np
import re
import collections

from .geometry_functions import cart2sph, rad2deg

#TODO add EQ configuration support
#TODO Add subwoofer configuration support
def createArrayConfigFile( outputFileName,
                           lspPositions,
                           twoDconfig = False,
                           sphericalPositions=False,
                           channelIndices = None,
                           loudspeakerLabels = None,
                           triplets = None,
                           lspDelays = None,
                           lspGainDB = None,
                           eqConfiguration = None,
                           virtualLoudspeakers = [],
                           subwooferConfig = [],
                           comment = None
                           ):
    """
    Generate a loudspeaker configuration XML file.

    Parameters
    ----------
    outputFileName: string
       The file name of the XML file to be written. This can be a file name or path.
       The file extension (typically .xml) must be provided by the user.
    lspPositions: array-like, 3xL or 2xL, where L is the number of loudspeakers.
       Provide the loudspeaker in Cartesian coordinates, relative to the centre of the array.
    twoDconfig: bool, optional
       Whether the loudspeaker configuration is 2D or 3D (default). In the former case, the lspPositions
       parameter does not need to have a third row, and it is ignored if present. If twoDconfig if True,
       then the loudspeaker coordinates in the do not have an "z" or "el" coordinate. Likewise,
       the triangulation "triplets" consist only of two loudspeakers.
    sphericalPositions: bool, optional
       Specify whether the loudspeaker and virtual loudspeaker positions are written in spherical (True)
       or Cartesian coordinates (False). Default is Cartesian.
    channelIndices: array-like, optional
       A list of output integer channel indices, one for each real loudspeaker. Optional argument, if not provided, consecutive indices starting from 1 are assigned. If provided, the length of the array must match the number of real loudspeakers, and indices must be unique.
    loudspeakerLabels: array-like, optional
       A list of strings containing alphanumerical labels for the real loudspeakers.
       Labels must be unique, consist of the characters 'a-zA-Z0-9&()\+:_-', one for
       each real loudspeaker. The labels are used to reference loudspeakers in triplets,
       virtual loudspeaker routings, and subwoofer configs.
       Optional parameter. If not provided, labels of the form 'lsp_i' with i=1,2,...
       are generated.
    triplets: array-like, optional.
       A loudspeaker triangulation. To be provided as a list of arrays consisting of three
       (or two in case of a 2D configuration) loudspeaker labels. Labels must match
       existing values of the loudspeakerLabels  parameter.
       Optional parameter, to be provided only in special cases. By default, the
       triangulation is computed internally.
    lspDelays: array-like, optional
       An array of delay values to be applied tothe loudspeakers. Values are to be provided in seconds.
       If not provided, no delays are applied. If specified, the length of the array must match the
       number of real loudspeakers.
    lspGainDB: array-like, optional.
       An array of gain values (in dB) to adjust the output gains of the real loudpeakers.
       If provided, the length must match the number of real loudspeakers.
       By default, no additional gains are applied.
    virtualLoudspeakers: array of dicts, optional
       Provide a set of additional virtual/phantom/dead/imaginary loudspeaker nodes to adjust the triangulation of the array. Each entry is a dict consisting of the following key-value pairs.

       * "id": A alphanumeric id, following the same rules as the loudspeaker indices. Must be unique across all real and imaginary loudspeakers.
       * "pos": A 3- or vector containing the position in Cartesian coordinates. 2 elements are allowed for 2D setups.
       * "routing": Specification how the panning gains calculated for this loudspeaker are distributed to neighbouring real loudspeakers. Provided as a list of tuples (label, gain), where label is the id of a real loudspeaker and gain is a linear gain value. Optional element, if not given, the energy of the virtual loudspeaker is discarded.

      Optional argument. No virtual loudspeakers are created if not specified.
    eqConfiguration: array of structures (dicts), optional
       Define a set of EQ filters to be applied to loudspeaker and subwoofer output channels. Each entry of the list is a dict containing the following key-value pairs.

       * "name": A unique, nonempty id that is referenced in loudspeaker and subwoofer specifications.
       * "filter": A list of biquad definitions, where each element is a dictionary containing the keys 'b' and 'a' that represent the numerator and denominator of the transfer function. 'b' must be a 3-element numeric vector, and 'a' a three- or two-element numeric vector. In the latter case, the leading coefficient is assumed to be 1, i.e., a normalised transfer function.
       * "loudspeakers": A list of loudspeaker labels (real loudspeakers) to whom the eq is applied.

    subwooferConfig: array of dicts, optional
       A list of subwoofer specifications, where each entry is a dictionary with the following key-value pairs:

       * "name": A string to name the subwoofer. If not provided, a default name will be generated.
       * "channel": An output channel number for the subwoofer signal. Must be unique across all loudspeakers and subwoofers.
       * "assignedSpeakers": A list of ids of (real) loudspeakers. The signals of these loudspeakers are used in the computation of the subwoofer signal.
       * "weights": An optional weighting applied to the loudspeaker signals of the the assigned loudspeakers. If provided, it must be an array-like sequence with the same length as assignedSpeakers. If not given, all assigned speakers are weighted equally with factor "1.0".

    comment: string, optional
       Optional string to be written as an XML comment at the head of the file.

    Examples
    ========

    A minimal example of a 3D configuration:

    .. code-block:: python 

       createArrayConfigFile( 'bs2051-4+5+0.xml',
                              lspPositions = lspPos,
                              twoDconfig = False,
                              sphericalPositions=True,
                              channelIndices = [1, 2, 3, 5, 6, 7, 8, 9, 10],
                              loudspeakerLabels =  ["M+030", "M-030", "M+000", "M+110", "M-110",
                                 "U+030", "U-030", "U+110", "U-110"  ],
                              virtualLoudspeakers = [ { "id": "VotD", "pos": [0.0, 0.0,-1.0],
                                                   "routing": [ ("M+030", 0.2), ("M-030", 0.2),
                                                    ("M+000", 0.2), ("M+110", 0.2), ("M-110", 0.2) ] }]

    """

    lspVecDim = lspPositions.shape[0]

    if len( lspPositions.shape ) != 2 or (lspVecDim not in [2,3]):
        raise ValueError( "The loudspeaker position data must be a 2D array with leading dimension 2 or 3" )

    numRealLoudspeakers = lspPositions.shape[-1]

    if lspVecDim == 2:
        lspPositions = np.concatenate( (lspPositions, np.zeros( (1, numRealLoudspeakers), dtype=lspPositions.dtype)), axis = 0 )

    if channelIndices is None:
        channelIndices = [ i+1 for i in range(numRealLoudspeakers) ]
    else:
        if len( channelIndices ) != numRealLoudspeakers:
            raise ValueError( "If the 'channelIndices' argument is given, its length must match the number of real loudspeakers." )

    if loudspeakerLabels is None:
        # Create default numbering 1...numRealLoudspeakers
        loudspeakerLabels = [ "lsp%d" % (i+1) for i in range(numRealLoudspeakers) ]
    else:
        if len( loudspeakerLabels ) != numRealLoudspeakers:
            raise ValueError( "If the 'loudspeakerLabels' argument is given, its length must match the number of real loudspeakers." )

    if virtualLoudspeakers is None:
        numVirtualLoudspeakers = 0
        virtualLoudspeakerPos = np.zeros( (3,0), dtype=lspPositions.dtype )
        virtualLoudspeakerLabels = []
    else:
        numVirtualLoudspeakers = len( virtualLoudspeakers )
        # TODO: Make the virtual loudspeaker ID optional
        virtualLoudspeakerLabels = [ record["id"] for record in virtualLoudspeakers ]
        virtualLoudspeakerPos = np.zeros( (3, numVirtualLoudspeakers), dtype=lspPositions.dtype )
        for (idx,record) in enumerate(virtualLoudspeakers):
            pos = np.asarray(record["pos"])
            # This also allows for 2D positions and sets the 3rd coordinate to 0 in this case.
            virtualLoudspeakerPos[0:pos.shape[-1],idx] = pos
            # Check optional downmix configuration for virtual loudspeakers.
            if "routing" in record:
                for routingEntry in record["routing"]:
                    downmixLabel =  routingEntry[0]
                    if downmixLabel not in loudspeakerLabels:
                        raise ValueError( "The label '%s' in the routing configuration of virtual loudspeaker '%s' does not denote a regular loudspeaker."
                                         % ( downmixLabel, virtualLoudspeakerLabels[idx] ) )

    allLoudspeakers = np.concatenate( (lspPositions, virtualLoudspeakerPos ), axis=-1 )
    allLabels = loudspeakerLabels + virtualLoudspeakerLabels

    # Check loudspeaker labels with regular expression and for uniqueness.
    labelRegex = re.compile( r"^[a-zA-Z0-9&\\(\)\\\+:_-]+$" )
    invalidLabels = []
    for label in loudspeakerLabels:
        if not bool(labelRegex.match(label) ):
            invalidLabels.append( label )
    if len( invalidLabels ) > 0:
        raise ValueError( "The loudspeaker labels '%s' are invalid." % str(invalidLabels ) )
    invalidVirtualLabels = []
    for label in virtualLoudspeakerLabels:
        if not bool(labelRegex.match(label) ):
            invalidVirtualLabels.append( label )
    if len( invalidVirtualLabels ) > 0:
        raise ValueError( "The virtual speaker labels '%s' are invalid." % str(invalidVirtualLabels ) )

    labelHist = collections.Counter( allLabels )
    duplicateLabels = [ label for label,count in labelHist.items() if count > 1 ]
    if len(duplicateLabels) > 0:
        raise ValueError( "Duplicate loudspeaker labels (real and virtual speakers) found: %s. "
                         % str(duplicateLabels) )

    if twoDconfig:
        allLoudspeakers = allLoudspeakers[:2,:]

    if triplets is None:
        # For the convex hull generation we have to normalise the loudspeaker vectors to unit distance,
        # i.e., to project them onto the unit sphere.

        Lnorms = np.linalg.norm( allLoudspeakers, ord=2, axis = 0 )
        Lnorm = allLoudspeakers / Lnorms[np.newaxis,:]

        hull = ConvexHull( Lnorm.T )
        triplets = hull.simplices
    else: # Use an existing triplet configuration, which is a list of label triplets.
        # TODO: Perform checking of the triplets
        pass

    labeledTriplets = [[ allLabels[i] for i in polygon ] for polygon in triplets ]

    if lspDelays is not None:
        if len(lspDelays) != numRealLoudspeakers:
            raise ValueError( "The argument 'lspDelays' is provided, but its length does not match the number of real loudspeakers." )
    if lspGainDB is not None:
        if len(lspGainDB) != numRealLoudspeakers:
            raise ValueError( "The argument 'lspDelays' is provided, but its length does not match the number of real loudspeakers." )


    # %% Create the document tree
    xmlRoot = ET.Element( "panningConfiguration" )
    if twoDconfig:
        xmlRoot.set( "dimension", "2")
    else:
        xmlRoot.set( "dimension", "3")

    if comment:
        xmlRoot.append( ET.Comment( comment ) )

    eqIdNames = set() # Check EQ ids for uniqueness
    if eqConfiguration: #  is not None:
        # At the moment we only support IIR filters.

        loudspeakerEqs = {} # Create dictionary mapping real loudspeakers to eq ids.

        maxOrder = 0
        outputEqNode = ET.SubElement( xmlRoot, "outputEqConfiguration" )

        for idx, eqSpec in enumerate( eqConfiguration ):
            eqNode = ET.SubElement( outputEqNode, "filterSpec" )
            eqId = eqSpec["name"]
            if eqId in eqIdNames:
                raise ValueError( "Duplicated eq ID '%s'" % eqId )
            eqIdNames.add( eqId )
            eqNode.set( "name", eqId )
            filterSpec = eqSpec["filter"]
            maxOrder = max( maxOrder, len( filterSpec ) )
            for biquadSpec in filterSpec:
                biqNode = ET.SubElement( eqNode, "biquad" )
                a = biquadSpec["a"]
                if len( a ) not in [2,3]:
                    raise ValueError( "In the EQ configuration '%s', the denominator (a) is not a 2- or 3-element array."
                                     % (eqId) )
                if len(a) >= 2:
                    biqNode.set( "a1", str(a[-2]) )
                    biqNode.set( "a2", str(a[-1]) )
                if len(a) == 3:
                    biqNode.set( "a0", str(a[0]) )
                b = biquadSpec["b"]
                if len( b ) != 3:
                    raise ValueError( "In the EQ configuration '%s', the denominator (b)) is not a 3-element array."
                                     % (eqId) )
                biqNode.set( "b0", str(b[0]) )
                biqNode.set( "b1", str(b[1]) )
                biqNode.set( "b2", str(b[2]) )
            if "loudspeakers" in eqSpec: # An EQ does not need to have associated EQs, for example a subwoofer crossover.
                assocLsp = eqSpec["loudspeakers"]
                for lsp in assocLsp:
                    if lsp not in loudspeakerLabels:
                        raise ValueError( "The loudspeaker label '%s' in the EQ specification '%' does not name an existing real loudspeaker."
                                         % (lsp, eqId) )
                    if lsp in loudspeakerEqs:
                        raise ValueError( "The loudspeaker label '%s' in the EQ specification '%' already has another EQ '."
                                         % (lsp, eqId) )
                    loudspeakerEqs[lsp] = eqId

        outputEqNode.set( "type", "iir" )
        outputEqNode.set( "numberOfBiquads", str(maxOrder) )

    for lspIdx in range(numRealLoudspeakers):
        lspNode = ET.SubElement( xmlRoot, "loudspeaker" )
        lspLabel = loudspeakerLabels[lspIdx]
        lspNode.set( "id", str(lspLabel) )
        lspNode.set( "channel", str(channelIndices[lspIdx]) )
        writePositionNode( lspNode,
                           lspPositions[:,lspIdx], sphericalPositions, twoDconfig )
        if lspDelays is not None:
            lspNode.set( "delay", str(round(lspDelays,10)) )
        if lspGainDB is not None:
            lspNode.set( "gainDB", str(round(lspGainDB,10)) )
        if eqConfiguration and (lspLabel in loudspeakerEqs):
            lspNode.set( "eq", loudspeakerEqs[lspLabel] )

    for (virtIdx, virtRecord) in enumerate( virtualLoudspeakers ):
        virtualLspNode = ET.SubElement( xmlRoot, "virtualspeaker" )
        virtualLspNode.set( "id", str(virtualLoudspeakerLabels[virtIdx]) )
        writePositionNode( virtualLspNode,
                           virtualLoudspeakerPos[:,virtIdx], sphericalPositions, twoDconfig )
        if "routing" in virtRecord:
            for routingEntry in virtRecord["routing"]:
                ( routingLabel, gain ) = routingEntry
                if routingLabel not in loudspeakerLabels:
                    raise ValueError( "Routing loudspeaker label %s does not match an existing (real) loudspeaker." % routingLabel )
                gainDB = 20.0*np.log10( gain )
                routingNode = ET.SubElement( virtualLspNode, "route" )
                routingNode.set( "lspId", str(routingLabel) )
                routingNode.set( "gainDB", str( round(gainDB,4) ) )

    for triplet in labeledTriplets:
        tripletNode = ET.SubElement( xmlRoot, "triplet" )
        # For the moment, we need triplets unless we have a 2d config.
        if (twoDconfig and len(triplet )!=2) or (not twoDconfig and len(triplet )!=3) :
            raise ValueError( "triplets must consist of eiter 3 vertices (£D setup) or two vertices (2D setup)" )
        tripletNode.set( "l1", str(triplet[0]) )
        tripletNode.set( "l2", str(triplet[1]) )
        if not twoDconfig:
            tripletNode.set( "l3", str(triplet[2]) )

    # %% subwoofer configuration
    if len(subwooferConfig) > 0:
        subChannels = set()
        for idx, subCfg in enumerate( subwooferConfig ):
            subNode = ET.SubElement( xmlRoot, "subwoofer" )
            subName = subCfg["name"] if "name" in subCfg else "sub_"+str(idx+1)
            subNode.set( "name", "subName" )
            subChannel = subCfg["channel"]
            if subChannel in channelIndices:
                raise ValueError( "The output channel %d used by subwoofer '%s' is already used as a loudspeaker channel."
                                 % (subChannel, subName) )
            if subChannel in subChannels:
                raise ValueError( "The output channel %d used by subwoofer '%s' is already used by another subwoofer."
                                 % (subChannel, subName) )
            subChannels.add( subChannel )

            subNode.set( "channel", str(subChannel) )
            assignedSpeakers = subCfg[ "assignedSpeakers" ]
            labelsNotFound = [ label for label in assignedSpeakers
                              if label not in loudspeakerLabels]
            if len( labelsNotFound ) > 0:
                raise ValueError( "Subwoofer configuration '%s% contains unknown associated speakers: %s"
                                 % (subName, str(labelsNotFound) ))
            assignedSpeakersString = ", ".join( str(x) for x in assignedSpeakers )
            subNode.set( "assignedLoudspeakers", assignedSpeakersString )
            if "weights" in subCfg:
                weights = subCfg["weights"]
                if len(weights) != len(assignedSpeakers):
                    raise ValueError( "Subwoofer configuration '%s' provides a field 'weights', but its length differs from the number of assigned speakers."
                                     % (subName))
                weightsString = ", ".join( str(round(x,10)) for x in weights )
                subNode.set( "weights", weightsString )
            if "gainDB" in subCfg:
                subNode.set( "gainDB", str(round(subCfg["gainDB"],4)) )
            if "delay" in subCfg:
                subNode.set( "delay", str(round(subCfg["delay"],6)) )
            if "eq" in subCfg:
                eqId = subCfg["eq"]
                if not eqId in eqIdNames:
                    raise ValueError( "Subwoofer configuration '%s' specifies the nonexistent EQ configuration '%s.'"
                                     % (subName, eqId ))
                subNode.set( "eq", eqId )

    # %% Write the array configuration.
    eTree = ET.ElementTree( element = xmlRoot )
    eTree.write( outputFileName, pretty_print=True )

def writePositionNode( xmlNode, pos, sphericalPositions, twoDconfig ):
    if sphericalPositions:
        az, el, radius = cart2sph( pos )
        sphNode = ET.SubElement( xmlNode, str("polar") )
        sphNode.set( "az", str(round(rad2deg( az ),10)) )
        if not twoDconfig:
            sphNode.set( "el",  str(round(rad2deg( el ),10)) )
        sphNode.set( "r", str(radius) )
    else:
        cartNode = ET.SubElement( xmlNode, "cart" )
        cartNode.set( "x", str(round(pos,10)) )
        cartNode.set( "y", str(round(pos,10)) )
        if not twoDconfig:
            cartNode.set( "z", str(pos) )

