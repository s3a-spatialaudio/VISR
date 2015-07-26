% Copyright (c) 2015, Andreas Franck, Institute of Sound and Vibration Research,
% University of Southampton, United Kingdom
% a.franck@soton.ac.uk

function  [outputString] = writeArrayConfigXml( fileName, spkPos, triplets, isInfinite, is2D, channelIndices, subwooferConfig )

subwooferConfigPresent = (nargin >= 7);

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


for spkIdx = 1:numSpeakers 
    isVirtual = (channelIndices( spkIdx ) < 1);
    if isVirtual
        spkNode = domNode.createElement( 'virtualspeaker' );
    else
        spkNode = domNode.createElement( 'loudspeaker' );
    end
    spkNode.setAttribute( 'id', num2str(spkIdx) );
    if ~isVirtual
        spkNode.setAttribute( 'channel', num2str(channelIndices(spkIdx) ));
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
        subNode.setAttribute( 'channel', num2str( subChannels(subIdx)) );
        subNode.setAttribute( 'assignedLoudspeakers', subIndices{subIdx } );
        if ~isempty( subGains )
            % val = num2str(subGains{subIdx});
            val = subGains{subIdx};
            subNode.setAttribute( 'weights', val );
        end
        rootNode.appendChild( subNode );
    end
end

if isempty(fileName) % Use [] to suppress a file output and write to the
    % output argument instead.
    outputString = xmlwrite( domNode );
else
    xmlwrite( fileName, domNode );
    outputString = [];
end

end % function
