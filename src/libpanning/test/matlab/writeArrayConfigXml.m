% writeArrayConfigXml - Write a loudspeaker configuration into an XML
% representation accepted by the VISR renderer
% Copyright (c) 2015, Andreas Franck, Institute of Sound and Vibration Research,
% University of Southampton, United Kingdom
% a.franck@soton.ac.uk
% Arguments:
% fileName: file name or path of the output file. If an empty string is
% provided, e.g., [], the XML representation is returned as a string.
% spkPos: The loudspeaker position in Cartesian coordinates as a Lx3 matrix
% (L is the number of real + virtual loudspeakers.
% triplets: The triangulation of the loudspeaker setup used by VBAP panning
% algorithms, size Tx3, where T denotes the number of triplets. The values
% are integer and represent loudspeaker indices corresponding to spkPos
% (one-offset)
% isInfinite: Boolean flag to denote whether the loudspeakers are assumed as
% plane waves (infinite distance). The effect on the reproduction algorithm
% is implementation-dependent.
% is2D: Boolean flag whether the setup should be considered as a horizontal
% 2D setup. In this case the third loudspeaker coordinate and the third
% speaker index are ignored.
% channelIndices: The audio interface output channels corresponding to the
% loudspeakers in spkPos. Indices are one-offset. Values < 1 are used to
% flag virtual/dead/phantom loudspeakers.
% subwooferConfig: A Matlab struct representing the subwoofer
% configuration. See the code and example scripts for the exact structure.
% Optional parameter.
% gainAdjust: Optional gain adjustments for the loudspeakers in dB. A
% vector of size L corresponding to the loudspeakers in spkPos.
% delayAdjust: Optional delay adjustments for the loudspeakers in seconds. A
% vector of size L corresponding to the loudspeakers in spkPos.
% Return value: the XML representation as a string if fileName is empty.
% TODO:
% - Provide 'real' 2D configs
% - Document subwoofer config
% - Optional downmix vectors for virtual loudspeakers
% - Incorporate equalisation config.

function  [outputString] = writeArrayConfigXml( fileName, spkPos, triplets, isInfinite, is2D, channelIndices, subwooferConfig, gainAdjust, delayAdjust, eqConfig )

subwooferConfigPresent = (nargin >= 7);

gainAdjustPresent = (nargin >= 8);
delayAdjustPresent = (nargin >= 9);

eqConfigPresent = (nargin >= 10 );

% Whether to write Cartesian or polar coordinates.
% TODO: Consider making this a configurable option.
useCartesian = false;

domNode = com.mathworks.xml.XMLUtils.createDocument('panningConfiguration');

rootNode = domNode.getDocumentElement;

if isInfinite
    infiniteStr = 'true';
else
    infiniteStr = 'false';
end
if is2D
    dimStr = '2';
else
    dimStr = '3';
end
rootNode.setAttribute( 'infinite', infiniteStr );
rootNode.setAttribute( 'dimension', dimStr );

numSpeakers = size( spkPos, 1 );
numTriplets = size( triplets, 1 );

if eqConfigPresent
    speakerEqLookup = zeros( 1, numSpeakers );

    eqNode = domNode.createElement( 'outputEqConfiguration' );

    for eqIdx = 1: length( eqConfig )
        
    end
end

for spkIdx = 1:numSpeakers 
    isVirtual = (channelIndices( spkIdx ) < 1);
    if isVirtual
        spkNode = domNode.createElement( 'virtualspeaker' );
    else
        spkNode = domNode.createElement( 'loudspeaker' );
    end
    spkNode.setAttribute( 'id', num2str(spkIdx) );
    if ~isVirtual
        chIdx = channelIndices(spkIdx);
        spkNode.setAttribute( 'channel', num2str( chIdx ));
        if gainAdjustPresent
            spkNode.setAttribute( 'gainDB', num2str(gainAdjust(chIdx)) );
        end
        if delayAdjustPresent
            spkNode.setAttribute( 'delay', num2str(delayAdjust(chIdx)) );
        end
    end
    if useCartesian
        coordNode = domNode.createElement( 'cart' );
        coordNode.setAttribute( 'x', num2str(spkPos(spkIdx, 1 )));
        coordNode.setAttribute( 'y', num2str(spkPos(spkIdx, 2 )));
        coordNode.setAttribute( 'z', num2str(spkPos(spkIdx, 3 )));
    else
        coordNode = domNode.createElement( 'polar' );
        [az, el, r ] = cart2sph( spkPos(spkIdx, 1 ), spkPos(spkIdx, 2 ), spkPos(spkIdx, 3 ) );
        coordNode.setAttribute( 'az', num2str(rad2deg(az)) );
        coordNode.setAttribute( 'el', num2str(rad2deg(el)) );
        coordNode.setAttribute( 'r', num2str(r) );
    end
    spkNode.appendChild( coordNode );
    rootNode.appendChild( spkNode );
end

if subwooferConfigPresent
    subChannels = subwooferConfig.channels;
    numSubwoofers = length( subChannels );
    
    subIndices = subwooferConfig.assignedLoudspeakers;
    if ~isvector( subIndices ) || (length( subIndices ) ~= numSubwoofers)
        display( 'The number of subwoofer index lists must match the number of subwoofers.' );
    end
    if isfield( subwooferConfig, 'loudspeakerWeights' )
        subGains = subwooferConfig.loudspeakerWeights;
        if ~isvector( subGains ) || length( subGains ) ~= numSubwoofers
            display( 'The number of subwoofer gain entries must match the number of subwooferss.' );
        end
    else
        subGains = [];
    end

    for subIdx = 1:numSubwoofers
        subNode = domNode.createElement( 'subwoofer' );
        chIdx = subChannels(subIdx);
        subNode.setAttribute( 'channel', num2str(chIdx+1) );
        if gainAdjustPresent
            subNode.setAttribute( 'gainDB', num2str(gainAdjust(chIdx)) );
        end
        if delayAdjustPresent
            subNode.setAttribute( 'delay', num2str(delayAdjust(chIdx)) );
        end
        subNode.setAttribute( 'assignedLoudspeakers', subIndices{subIdx } );
        if ~isempty( subGains )
            % val = num2str(subGains{subIdx});
            val = subGains{subIdx};
            subNode.setAttribute( 'weights', val );
        end
        rootNode.appendChild( subNode );
    end
end

for tripletIdx = 1:numTriplets
    % Do not save 'unused triplets' as permitted by the txt config format.
    if any( triplets(tripletIdx) < 1 )
       continue;
    end
    triplNode = domNode.createElement( 'triplet' );
    triplNode.setAttribute( 'l1', num2str( triplets(tripletIdx,1) ) );
    triplNode.setAttribute( 'l2', num2str( triplets(tripletIdx,2) ) );
    if ~is2D % Do not save 3rd vertex for a 2D setup
        triplNode.setAttribute( 'l3', num2str( triplets(tripletIdx,3) ) );
    end
    rootNode.appendChild( triplNode );
end

if isempty(fileName) % Use [] to suppress a file output and write to the
    % output argument instead.
    outputString = xmlwrite( domNode );
else
    xmlwrite( fileName, domNode );
    outputString = [];
end

end % function
