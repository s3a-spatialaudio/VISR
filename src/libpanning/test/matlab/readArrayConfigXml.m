% Copyright (c) 2015, Andreas Franck, Institute of Sound and Vibration Research,
% University of Southampton, United Kingdom
% a.franck@soton.ac.uk

function [spkPos, triplets, isInfinite, is2D, channelIndices] = readArrayConfigXml( fileName )

rootNode = xmlread( fileName );

configNodes = rootNode.getElementsByTagName( 'panningConfiguration' );
if configNodes.getLength() ~= 1
    error( 'The XML file must contain exactly one "panningConfiguration" element.' );
end
configNode = configNodes.item(0);

isInfinite = false;
if configNode.hasAttribute( 'infinite' );
    infStr = configNode.getAttribute( 'infinite' );
    isInfinite = xmlBool2logical( infStr );
end

is2D = false;
if configNode.hasAttribute( 'dimension' );
    dimStr = configNode.getAttribute( 'dimension' );
    if strcmpi( dimStr, '2' )
        is2D = true;
    elseif strcmpi( dimStr, '3' )
        is2D = false;
    else
        error( 'The optional attribute "dimension" must be either "2" or "3".' );
    end
end

speakerNodes = configNode.getElementsByTagName( 'loudspeaker' );
numSpeakers = speakerNodes.getLength();

virtualSpeakerNodes = configNode.getElementsByTagName( 'virtualspeaker' );
numVirtualSpeakers = virtualSpeakerNodes.getLength();

numTotalSpeakers = numSpeakers + numVirtualSpeakers;

spkPos = zeros( numTotalSpeakers, 3 );
channelIndices = zeros( 1, numSpeakers );

for spkIdx = 1:numSpeakers
    [ spkPos( spkIdx, : ), channelIndices(spkIdx) ] ...
        = parseSpeaker( speakerNodes.item( spkIdx-1), false );
end

for spkIdx = 1:numVirtualSpeakers
    spkPos( numSpeakers+spkIdx, : ) ...
        = parseSpeaker( virtualSpeakerNodes.item( spkIdx-1), true );
end

tripletNodes = configNode.getElementsByTagName( 'triplet' );
numTriplets = tripletNodes.getLength();
triplets = zeros( numTriplets, 3 );
for tripletIdx = 1: numTriplets
    tripletNode = tripletNodes.item(tripletIdx-1);
    l1str = tripletNode.getAttribute( 'l1' );
    l1 = str2double( l1str );
    l2 = str2double( tripletNode.getAttribute( 'l2' ));
    if ~tripletNode.hasAttribute( 'l3' )
        if is2D
            l3 = -1; % Default value assumed by renderer
        else
            error( 'For a 3D setup, all three triplet elements must be present.');
        end
    else
        % If the attribute is given, read whatever what is there regardless
        % whether we have a 2D setup.
        l3 = str2double( tripletNode.getAttribute( 'l3' ));
    end
    triplets( tripletIdx, : ) = [l1 l2 l3];
end % tripletIdx loop

end % function readArrayConfigXml

function [pos, chIdx ] = parseSpeaker( domNode, isVirtual )
cartNodes = domNode.getElementsByTagName( 'cart' );
sphericalNodes = domNode.getElementsByTagName( 'polar' );
numCartesianNodes = cartNodes.getLength();
numSphericalNodes = sphericalNodes.getLength();
if numCartesianNodes + numSphericalNodes ~= 1
    error( 'Loudspeaker node must contain either one "cart" or "polar" element.' );
end
if numCartesianNodes == 1
    cartNode = cartNodes.item(0);
    x  = str2double(cartNode.getAttribute( 'x' ));
    y  = str2double(cartNode.getAttribute( 'y' ));
    z  = str2double(cartNode.getAttribute( 'z' ));
else
    sphNode = sphericalNodes.item(0);
    az  = str2double(sphNode.getAttribute( 'az' ));
    el  = str2double(sphNode.getAttribute( 'el' ));
    r  = str2double(sphNode.getAttribute( 'r' ));
    [x, y, z ] = sph2cart( deg2rad(az), deg2rad(el), r );
end
pos = [x y z];

if ~isVirtual
    chIdxStr = domNode.getAttribute( 'channel');
    chIdx = str2double( chIdxStr );
end
end

function v = xmlBool2logical( str )
if strcmpi( str, 'true' ) || strcmp( str, '1' )
    v = true;
elseif strcmpi( str, 'false' ) || strcmp( str, '0' )
    v = false;
else
    error( 'Boolean XML attribute must be either "true", "false", "1" or "0" (case-insensitive).' );
end
end

