% Copyright (c) 2015, Andreas Franck, Institute of Sound and Vibration Research,
% University of Southampton, United Kingdom
% a.franck@soton.ac.uk

% Script to generate the XML configuratons for the audio lab.

% Ensure that $VISR/src/libpanning/test/matlab is in the path.

lspDataFile = '../audiolab_gain_delay.xls';

lspData = xlsread( lspDataFile );
numAllSpeakers = size( lspData, 1 );

%% The configurations
for numConfig = 1:8
    switch numConfig
        case 1 % Full setup without subwoofer
            configName = 'audiolab_39speakers';
            channels = 1:39;
            subChannels = [];
            subIndices = {};
            subGains = {};
            subGainAdjustDB = [];
            subDelayAdjust = [];
            
            is2D = false;
            isInfinite = false;
            
            % No virtual loudspeakers
            virtualSpeakers = [];

        case 2 % Full setup with subwoofer
            configName = 'audiolab_39speakers_1subwoofer';
            channels = 1:39;      % one-offset
            subChannels = [ 40 ]; % zero-offset
            subIndices = {0:38};  % zero-offset
            subGains = {ones(size(channels))};
            subGainAdjustDB = [ 0 ];
            subDelayAdjust = [0 ];

            is2D = false;
            isInfinite = false;

            % No virtual loudspeakers
            virtualSpeakers = [];

        case 3 % BS-20152 setup 9+10+3
            configName = 'audiolab_22speakers';
            channels = [ ... ;      % one-offset channel indices
             1,... % VoG
             6, 8, 9, 10, 11, 12, 13, 14, ...
             16, 17, 18, 19, 20, 22, 24, 25, 26, 27,...
             28, 29, 35,...
            ];
            subChannels = []; % zero-offset
            subIndices = {[]};  % zero-offset
            subGains = {[]};
            subGainAdjustDB = [];
            subDelayAdjust = [];

            is2D = false;
            isInfinite = false;
            
            virtualSpeakers = [0, 0, -1];

        case 4 % BS-20152 setup 9+10+3
            configName = 'audiolab_22speakers_1subwoofer';
            channels = [ ... ;      % one-offset channel indices
             1,... % VoG
             6, 8, 9, 10, 11, 12, 13, 14, ...
             16, 17, 18, 19, 20, 22, 24, 25, 26, 27,...
             28, 29, 35,...
            ];
            subChannels = [ 40 ]; % zero-offset
            subIndices = {0:21};  % zero-offset
            subGains = {ones(size(channels))};
            subGainAdjustDB = [ 0 ];
            subDelayAdjust = [0 ];

            is2D = false;
            isInfinite = false;
            
            virtualSpeakers = [0, 0, -1];
        case 5 % 4+5+0 setup
            configName = '9.1_audiolab_1subwoofer';
            channels = [ ... ;      % one-offset channel indices
                7, 10, 12, 15,...
                16, 17, 20, 24, 27 ...
            ];
            subChannels = [ 40 ]; % zero-offset
            subIndices = {0:8};  % zero-offset
            subGains = {ones(size(channels))};
            subGainAdjustDB = [ 0 ];
            subDelayAdjust = [0 ];

            is2D = false;
            isInfinite = false;
            
            virtualSpeakers = [0, 0, -1];
            
        case 6 % 0+5+0
            configName = '5.1_audiolab_1subwoofer';
            channels = [ ... ;      % one-offset channel indices
                 27, 16, 17, 20, 24 ...
            ];
            subChannels = [ 40 ]; % zero-offset
            subIndices = {0:4};  % zero-offset
            subGains = {ones(size(channels))};
            subGainAdjustDB = [ 0 ];
            subDelayAdjust = [ 0 ];

            is2D = true;
            isInfinite = false;
                        
        case 7 % 0+2+0 (vulgo stereo)
            configName = 'stereo_audiolab_1subwoofer';
            channels = [ ... ;      % one-offset channel indices
                 27, 17, ...
            ];
            subChannels = [ 40 ]; % zero-offset
            subIndices = {0:1};  % zero-offset
            subGains = {ones(size(channels))};
            subGainAdjustDB = [ 0 ];
            subDelayAdjust = [0 ];

            is2D = true;
            isInfinite = false;
            
            virtualSpeakers = [-1, 0, 0];
            
        case 8
            configName = 'cube_audiolab_1subwoofer';
            channels = [ ... ;      % one-offset channel indices
                 8,10,12,14,...
                 29, 31, 33, 35 ...
            ];
            subChannels = [ 39 ]; % zero-offset
            subIndices = {0:7};  % zero-offset
            subGains = {ones(size(channels))};
            subGainAdjustDB = [ 0 ];
            subDelayAdjust = [0 ];

            is2D = false;
            isInfinite = false;
            
            virtualSpeakers = [];
    end
    
    numRegularSpeakers = length(channels);    
    numVirtualSpeakers = size(virtualSpeakers,1);
    numAllSpeakers = numRegularSpeakers + numVirtualSpeakers;
    virtualSpeakerIndices = (1:numVirtualSpeakers) + numRegularSpeakers;


    usedSpeakers = zeros( numAllSpeakers, 3 );
    finalChannelIndices = zeros( numAllSpeakers, 1 );
    
    % Reverse lookup from channels to speakers.
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
        % In case virtual loudspeakers are contained in the table (denoted
        % by channel index -1 or 0)
        if( findIdx >= 0 )
            finalChannelIndices( chIdx ) = channels( chIdx ); % No offset here for some reason
            if channels( chIdx ) >= 0
                channelToSpeakerIndex( channels( chIdx ) ) = chIdx;
            end
        else
            finalChannelIndices( chIdx ) = 0; % Special value to denote a virtual speaker
        end
    end
    for virtIdx = 1:numVirtualSpeakers
        usedSpeakers( numRegularSpeakers + virtIdx, : ) = virtualSpeakers(virtIdx,:);
        finalChannelIndices( numRegularSpeakers + virtIdx ) = 0; % mark it as virtual loudspeaker
        channelToSpeakerIndex( channels( chIdx ) ) = 0; % Don't know whether this is really necessary anymore.
    end
    
    if is2D
        % Horizontal 2D setups
        % Simply sort edges by the azimuth angles
        spkAz = atan2( usedSpeakers(:,2), usedSpeakers(:,1) );
        [~,sortIdx] = sort( spkAz );
        % Create segment of consecutive loudspeakers
        triplets = [sortIdx, circshift( sortIdx, 1 ) zeros(size(usedSpeakers,1), 1 ) ];
    else
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
        %     figName = [configName '.png'];
        %     print( 1, figName, '-dpng' );
    end
    
    %% Create the subwoofer configuration
    numSubs = length( subChannels );
    
    subSpeakerIndices = cell( numSubs, 1 );
    subSpeakerGains = cell( numSubs, 1 );
    for subIdx = 1:numSubs
        speakerIndices = subIndices{subIdx} + 1;
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
    
    gainAdjustDB = zeros(size(lspData( :, 24 )));
    delayAdjust = zeros(size(lspData( :, 22 )))*1e-3; % table values in ms
    
    % Todo: Set any adjustments to the gains and delays here.
    
    % Set the already defined gains and delays for the loudspeakers
    gainAdjustDB( subChannels ) = subGainAdjustDB;
    delayAdjust( subChannels ) = subDelayAdjust;
    
    outputFileXml = ['../'  configName '.xml'];
    
    writeArrayConfigXml( outputFileXml, usedSpeakers, triplets, isInfinite, is2D, finalChannelIndices, subwooferConfig, gainAdjustDB, delayAdjust );
    
end % configuration loop
