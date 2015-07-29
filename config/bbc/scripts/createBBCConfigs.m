% Copyright (c) 2015, Andreas Franck, Institute of Sound and Vibration Research,
% University of Southampton, United Kingdom
% a.franck@soton.ac.uk

% Script to generate the BBC configuratons.

% Ensure that $VISR/src/libpanning/test/matlab is in the path.

lspDataFile = '../speaker_config_original/listeningroomspeakers.txt';

lspData = load( lspDataFile, '-ascii' );
numAllSpeakers = size( lspData, 1 );

%% The configurations

for numConfig = 1:7
    
    switch numConfig
        case 1 % 9 + 10 + 3
            configName = 'bs2051-9+10+3';
            channels = [0 1 2 ...
                6 7 8 10 11 13 14 15 17 18 ...
                19 21 23 24 26 27 28 30 31 -1];
            subChannels = [32 33];
            subIndices = { [0 1 2     11 10 8 7 6 18 17    24 23 21 19 30   31],...
                [11 13 14 15 17   24 26 27 28 30   31] };
            subGains = { [1 1 1   0.5 1 1 1 1 1 0.5   0.5 1 1 1 0.5   0.5],...
                [0.5 1 1 1 0.5   0.5 1 1 1 0.5   0.5] };
            subGainAdjustDB = [ -3.5, -6.5 ];
            subDelayAdjust = [ 1e-3, 2.35e-3];
            
            is2D = false;
            isInfinite = false;
        case 2 % 4 + 9 + 0
            configName = 'bs2051-4+9+0';
            channels = [7 8 6 11 17 13 15 23 19 25 29 -1];
            subChannels = [32 33];
            subIndices = { [11 8 7 6 17 23 19],...
                [11 13 15 17 25 29] };
            subGains = { [0.5 1 1 1 0.5 1 1],...
                [0.5 1 1 0.5 1 1] };
            subGainAdjustDB = [ -3.5, -6.5 ];
            subDelayAdjust = [ 1e-3, 2.35e-3];
            is2D = false;
            isInfinite = false;
        case 3 % 4 + 5 + 0
            configName = 'bs2051-4+5+0';
            channels = [7 8 6 12 16 22 20 25 29 -1];
            subChannels = [32 33];
            subIndices = { [12 8 7 6 16 20 22 25 29],...
                [12 16 25 29] };
            subGains = { [0.5 1 1 1 0.5 1 1 0.5 0.5],...
                [0.5 0.5 0.5 0.5] };
            subGainAdjustDB = [ -3.5, -6.5 ];
            subDelayAdjust = [ 1e-3, 2.35e-3];
            is2D = false;
            isInfinite = false;
        case 4 % 2 + 5 + 0
            configName = 'bs2051-2+5+0';
            channels = [7 8 6 12 16 22 20 -1];
            subChannels = [32 33];
            subIndices = { [7 8 6 22 20],...
                [12 16] };
            subGains = { [1 1 1 1 1],...
                [1 1] };
            subGainAdjustDB = [ -3.5, -6.5 ];
            subDelayAdjust = [ 1e-3, 2.35e-3];
            is2D = false;
            isInfinite = false;
        case 5 % 0 + 5 + 0
            % The automatic triangulation fails here, so we include a
            % virtual loudspeaker here.
            % The configuration has to be amended afterwards.
            configName = 'bs2051-0+5+0';
            channels = [7 8 6 12 16 -1];
            subChannels = [32 33];
            subIndices = { [7 8 6],...
                [12 16] };
            subGains = { [1 1 1],...
                [1 1] };
            subGainAdjustDB = [ -3.5, -6.5 ];
            subDelayAdjust = [ 1e-3, 2.35e-3];
            is2D = true; % Normally, this is a planar setup, handled separately by the VISR renderer
            isInfinite = false;
            % The automatic triangulation probably fails.
        case 6 % 0 + 2 + 0 aka stereo
            % Automatic approach does not work at all
            continue;
            configName = 'bs2051-0+2+0';
            channels = [7 6];
            subChannels = [32 33];
            subIndices = { [7 6],...
                [7 6] };
            subGains = { [0.5 0.5],...
                [0.5 0.5] };
            subGainAdjustDB = [ -3.5, -6.5 ];
            subDelayAdjust = [ 1e-3, 2.35e-3];
            is2D = true; % This is a planar setup, handled separately by the VISR renderer
            isInfinite = false;
            % The automatic triangulation most probably fails.
            
        case 7 % full listening room
            configName = 'bbc-listening-room-full';
            channels = 0:31;
            subChannels = [32 33];
            subIndices = { [11 24 31 30 17 19 20 21 22 23 18 5 6 7 8 9 10 0 1 2],...
                [11 24 31 30 17 25 26 27 28 29 12 13 14 15 16 3 4] };
            subGains = { [0.5*ones(1,5) ones(1,15) ],...
                [0.5*ones(1,5) ones(1,12)] };
            subGainAdjustDB = [ -3.5, -6.5 ];
            subDelayAdjust = [ 1e-3, 2.35e-3];
            is2D = false;
            isInfinite = false;
    end
    
    usedSpeakers = zeros( length( channels), 3 );
    finalChannelIndices = zeros( length(channels), 1 );
    
    % Reverse lookup from channels to spekers.
    % Note: we have to add 1 to the speakers due to Matlab indexing.
    channelToSpeakerIndex = NaN( max( channels )+1, 1 );
    
    for chIdx = 1:length(channels)
        findIdx = find( lspData(:,1) == channels( chIdx ) );
        if ~isscalar( findIdx )
            error( 'Each referenced loudspeaker index must exist exactly once.' );
        end
        [x, y, z] = sph2cart( degtorad(lspData(findIdx, 2 )), degtorad(lspData(findIdx, 3 )), lspData(findIdx, 4 ) );
        usedSpeakers( chIdx, : ) = [x y z];
        if( findIdx >= 0 )
            finalChannelIndices( chIdx ) = channels( chIdx );
            if channels( chIdx ) >= 0
                channelToSpeakerIndex( channels( chIdx ) + 1 ) = chIdx;
            end
        else
            finalChannelIndices( chIdx ) = Inf; % Special value to denote a virtual speaker
        end
    end
    
    spkCoordsNorm = usedSpeakers ./ repmat( sqrt(sum(usedSpeakers.^2, 2)), 1, 3);
    DT = delaunayTriangulation(spkCoordsNorm); % This computes a partitioning of the volume into tetrahedra
    [triplets,~] = convexHull( DT );
    
    figure(1);
    hold off;
    trisurf(triplets, DT.Points(:,1), DT.Points(:,2), DT.Points(:,3) );
    xlabel('x [m]');ylabel('y [m]');zlabel('z [m]');
    axis equal;
    labels = num2str( finalChannelIndices,'%d');    %'
    text(DT.Points(:,1), DT.Points(:,2), DT.Points(:,3), labels, 'horizontal','left', 'vertical','bottom', 'FontSize', 14);
    figName = [configName '.png'];
    print( 1, figName, '-dpng' );
    
    %% Create the subwoofer configuration
    numSubs = length( subChannels );
    
    subSpeakerIndices = cell( numSubs, 1 );
    subSpeakerGains = cell( numSubs, 1 );
    for subIdx = 1:numSubs
        speakerIndices = channelToSpeakerIndex( subIndices{subIdx} + 1 );
        if ~all(isfinite(speakerIndices))
            error('Subwoofer configuration: Loudspeaker index lookup revealed invalid assigned speaker indices..');
        end
        if length( speakerIndices ) ~= length(subGains{subIdx} )
            error('Subwoofer configuration: The length of the assigned speaker index vector and the weigths must match.');
        end
        
        subIndicesStr = sprintf( ', %d', speakerIndices );
        subIndicesStr = subIndicesStr(3:end); % Remove the initial ', '
        subSpeakerIndices(subIdx) = { subIndicesStr };
        subGainsStr = sprintf( ', %f', subGains{subIdx} );
        subGainsStr = subGainsStr(3:end);
        subSpeakerGains(subIdx) = { subGainsStr };
    end
    
    subwooferConfig = struct( 'channels', subChannels,...
        'assignedLoudspeakers', {subSpeakerIndices},...
        'loudspeakerWeights', {subSpeakerGains} );
    
    %% Configure the channel adjustments
    numTotalChannels = max(max(finalChannelIndices+1), max(subChannels+1));
    
    gainAdjustDB = zeros(1,numTotalChannels );
    delayAdjust = zeros(1,numTotalChannels );
    
    % Todo: Set any adjustments to the gains and delays here.
    
    % Set the already defined gains and delays for the loudspeakers
    gainAdjustDB( subChannels ) = subGainAdjustDB;
    delayAdjust( subChannels ) = subDelayAdjust;
    
    outputFileXml = ['../'  configName '.xml'];
    
    writeArrayConfigXml( outputFileXml, usedSpeakers, triplets, isInfinite, is2D, finalChannelIndices, subwooferConfig, gainAdjustDB, delayAdjust );
    
end % configuration loop
