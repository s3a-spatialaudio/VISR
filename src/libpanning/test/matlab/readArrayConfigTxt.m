% Copyright (c) 2015, Andreas Franck, Institute of Sound and Vibration Research,
% University of Southampton, United Kingdom
% a.franck@soton.ac.uk

% readArrayConfigTxt - Read the text-based configuration format for the
% VISR VBAP functionality
% fileName - name (possibly path) of config file.
% Return values:
% spkPos - Cartesian loudspeaker coordinates, 1 speaker per column
% triplets - loudspeaker indices (1-offset) forming the triplets of the triangulation,
% 1 triplet per row. Might contain 'unused' triplets with ''-1' values.
% isInfinite - Whether the configuration is 'infinite', i.e., assuming
% plane waves from the speakers.

function [spkPos, triplets, isInfinite, is2D, channelIndices ] = readArrayConfigTxt( fileName )

hFile = fopen( fileName, 'r' );

if hFile < 0
    error( 'File could not be opened for reading.' );
end

% Constants taken from the current linitations on maximum number of
% speakers and triplets.
maxNumSpeakers = 128;
maxNumTriplets = 256;

% In the current implementation, the maximum index for 
maxSpeakerIndex = 0;
maxTripletIndex = 0;

spkPosAll =   NaN( maxNumSpeakers, 3 );
tripletsAll = -ones( maxNumTriplets, 3 ); % '-1' denotes an unued triplet.
channelsAll = NaN( maxNumSpeakers, 1 );

isInfinite = false;
is2D = false;

while ~feof( hFile )
    lineStr = fgets( hFile );
    if isempty( lineStr ) 
        continue;
    end
    switch lineStr(1)
        case '%' % Skip comment lines
            continue;
        case 'i'
            isInfinite = true;
        case '2'
            is2D = true;
        case 'c' % Scan Cartesian coordinates.
            [A,count ] = sscanf( lineStr(2:end), '%d %d %f %f %f' );
            if count ~= 5
                error( 'Error parsing Cartesian coordinates.' );
            end
            spkIdx = A(1);
            chanIdx = A(2);
            if ~all(isnan(spkPosAll( spkIdx, : )))
                error( 'Error parsing Cartesian coordinates: Assigning already used speaker index' );
            end
            spkPosAll( spkIdx, : ) = A(3:5);
            channelsAll( spkIdx ) = chanIdx;
            maxSpeakerIndex = max( maxSpeakerIndex, spkIdx );
        case 'p' % Scan polar coordinates.
            [A,count ] = sscanf( lineStr(2:end), '%d %d %f %f %f' );
            if count ~= 5
                error( 'Error parsing Polar coordinates.' );
            end
            spkIdx = A(1);
            chanIdx = A(2);
            if ~all(isnan(spkPosAll( spkIdx, : )))
                error( 'Error parsing Cartesian coordinates: Assigning already used speaker index' );
            end
            [x, y, z ] = sph2cart( degtorad(A(3)), degtorad(A(4)), A(5) );
            spkPosAll( spkIdx, : ) = [x y z];
            channelsAll( spkIdx ) = chanIdx;
            maxSpeakerIndex = max( maxSpeakerIndex, spkIdx );
        case 't' % Scan triplet definition.
            [A,count ] = sscanf( lineStr(2:end), '%d %d %d %d' );
            if (count < 3) || (count > 4 );
                error( 'Error parsing triplet definition.' );
            end
            triplIdx = A(1);
            if ~all(tripletsAll( triplIdx, : ) == -1) % Unassigned yet?
                error( 'Error parsing triplet definition: Assigning already used triplet index' );
            end
            if count == 3 % Must be a 2D setup, but we cannot check this yet, 
            % as is it unclear at which point in the file the '2D' flag is set.  
                tripletsAll( triplIdx, : ) = [A(2:3); 0]; % Assign predefined value to 3rd triplet index.
            else
                tripletsAll( triplIdx, : ) = A(2:4);
            end
            maxTripletIndex = max( maxTripletIndex, triplIdx );
    end
end
fclose( hFile );

% Prune the output matrices to the dimension of the maximum speaker/triplet
% index.
spkPos = spkPosAll(1:maxSpeakerIndex,:);
triplets = tripletsAll(1:maxTripletIndex,:);
channelIndices = channelsAll( 1:maxSpeakerIndex,:);

if any( isnan( spkPos ))
    error( 'Unassigned speaker positions' );
end
% No checking for unassigned triplets, as the current format supports these
% as a quick way to comment/exchange different variants.

end
