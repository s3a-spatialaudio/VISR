% Copyright (c) 2015, Andreas Franck, Institute of Sound and Vibration Research,
% University of Southampton, United Kingdom
% a.franck@soton.ac.uk

% Script to generate the BBC configuratons.

% Ensure that $VISR/src/libpanning/test/matlab is in the path.

lspDataFile = '../audiolab_gain_delay.xls';

lspData = xlsread( lspDataFile );
numAllSpeakers = size( lspData, 1 );

%% The configurations

for numConfig = 1:1
    
    switch numConfig
        case 1 % 9 + 10 + 3
            configName = 'audiolab_39speakers';
            channels = 1:39;
            subChannels = [];
            subIndices = {};
            subGains = {};
            subGainAdjustDB = [];
            subDelayAdjust = [];
            
            is2D = false;
            isInfinite = false;
    end
    
    usedSpeakers = zeros( length( channels), 3 );
    finalChannelIndices = zeros( length(channels), 1 );
    
    % Reverse lookup from channels to spekers.
    % Note: we have to add 1 to the speakers due to Matlab indexing.
    channelToSpeakerIndex = NaN( max( channels )+1, 1 );
    
    for chIdx = 1:length(channels)
        findIdx = find( lspData(:,13) == channels( chIdx ) );
        if ~isscalar( findIdx )
            error( 'Each referenced loudspeaker index must exist exactly once.' );
        end
        % [x, y, z] = sph2cart( degtorad(lspData(findIdx, 2 )), degtorad(lspData(findIdx, 3 )), lspData(findIdx, 4 ) );
        x = lspData( findIdx, 14 );
        y = lspData( findIdx, 15 );
        z = lspData( findIdx, 16 );
        usedSpeakers( chIdx, : ) = [x y z];
        if( findIdx >= 0 )
            finalChannelIndices( chIdx ) = channels( chIdx ); % No offset here for some reason
            if channels( chIdx ) >= 0
                channelToSpeakerIndex( channels( chIdx ) ) = chIdx;
            end
        else
            finalChannelIndices( chIdx ) = 0; % Special value to denote a virtual speaker
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
    if isempty(subChannels)
        numTotalChannels = max(finalChannelIndices);
    else
        numTotalChannels = max(max(finalChannelIndices), max(subChannels+1));
    end
    
    gainAdjustDB = lspData( :, 24 );
    delayAdjust = lspData( :, 22 )*1e-3; % table values in ms
    
    % Todo: Set any adjustments to the gains and delays here.
    
    % Set the already defined gains and delays for the loudspeakers
    gainAdjustDB( subChannels ) = subGainAdjustDB;
    delayAdjust( subChannels ) = subDelayAdjust;
    
    outputFileXml = ['../'  configName '.xml'];
    
    writeArrayConfigXml( outputFileXml, usedSpeakers, triplets, isInfinite, is2D, finalChannelIndices, subwooferConfig, gainAdjustDB, delayAdjust );
    
end % configuration loop
