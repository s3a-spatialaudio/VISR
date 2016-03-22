% Copyright (c) 2015, Andreas Franck, 
% Institute of Sound and Vibration Research,
% University of Southampton, United Kingdom
% a.franck@soton.ac.uk

% Script to generate the BBC configuratons.

% Ensure that $VISR/src/libpanning/test/matlab is in the path.
% TODO: Add more configurations as required
% TODO: Add subwoofer configs where necessary.
% TODO: Add support for variable output channel indices.

allSpeakerCoordsSpherical = [...
 0 0;...      
 -22.5 0;...
 +22.5 0;...
 -15 0;...
 +15 0;...
 -30 0;...
 +30 0;...
 -45 0;...
 +45 0;...
 -60 0;...
 +60 0;...
 -90 0;...
 +90 0;...
 -110 0;...
 +110 0;...
 -135 0;...
 +135 0;...
 +180 0;...
 0 +30;...
 -22.5 +30;...
 +22.5 +30;...
 -30 +30;...
 +30 +30;...
 -45 +30;...
 +45 +30;...
 -60 +30;...
 +60 +30;...
 -90 +30;...
 +90 +30;...
 -110 +30;...
 +110 +30;...
 -135 +30;...
 +135 +30;...
 +180 +30;...
 0 +90;...
 0 -30;...
 -22.5 -30;...
 +22.5 -30;...
 -30 -30;...
 +30 -30;...
 -45 -30;...
 +45 -30;...
 -60 -30;...
 +60 -30;...
 -90 -30;...
 +90 -30;...
 -110 -30;...
 110 -30;...
 -135 -30;...
 135 -30;...
 +180 -30;...
-45 -30;...
+45 -30;...
];

allSpeakerLabels = {...
'M+000',... %  1
'M-022',... %  2
'M+022',... %  3
'M-SC',...  %  4
'M+SC',...  %  5
'M-030',... %  6
'M+030',... %  7
'M-045',... %  8
'M+045',... %  9
'M-060',... % 10
'M+060',... % 11
'M-090',... % 12
'M+090',... % 13
'M-110',... % 14
'M+110',... % 15
'M-135',... % 16
'M+135',... % 17
'M+180',... % 18
'U+000',... % 19
'U-022',... % 20
'U+022',... % 21
'U-030',... % 22
'U+030',... % 23
'U-045',... % 24
'U+045',... % 25
'U-060',... % 26
'U+060',... % 27
'U-090',... % 28
'U+090',... % 29
'U-110',... % 30
'U+110',... % 31
'U-135',... % 32
'U+135',... % 33
'U+180',... % 34
'T+000',... % 35
'B+000',... % 36
'B-022',... % 37
'B+022',... % 38
'B-030',... % 39
'B+030',... % 40
'B-045',... % 41
'B+045',... % 42
'B-060',... % 43
'B+060',... % 44
'B-090',... % 45
'B+090',... % 46
'B-110',... % 47
'B+110',... % 48
'B-135',... % 49
'B+135',... % 50
'B+180',... % 51
'LFE1',...  % 52
'LFE2',...  % 53
};

numAllSpeakers = size( allSpeakerCoordsSpherical, 1 );
[x, y, z] = sph2cart( deg2rad(allSpeakerCoordsSpherical(:,1)),...
                     deg2rad(allSpeakerCoordsSpherical(:,2)), 1);
allSpeakersCart = [x, y, z];

%% The configurations
for numConfig = 1:1
    
    switch numConfig
        case 1 % 9 + 10 + 3
            configName = 'bs2051-9+10+3';
            % Note: all indices are one-offset 
            speakerIndices = [11 10 1 17 16 7 6 18,...
                13 12 25 24 19 35 33 32 29 28 34 36 42 41];
            % Add virtual speakers as a matrix where each row contains a
            % Cartesian coordinate.
            virtualSpeakers = [0, 0, -1];
            channels = [1:3 5:9 11:24];
            % Subwoofer support not implemented yet 
%             subChannels = [];
%             leftSpeakers =  [11 17 7 13 25 33 29 42];
%             rightSpeakers = [10 16 6 12 24 32 28 41];
%             medianSpeakers = [1 18 19 35 34 36];
%             subIndices = { [leftSpeakers medianSpeakers], [rightSpeakers medianSpeakers] };
%             subGainsLeft = [ones(size(leftSpeakers)), 0.5*ones(size(rightSpeakers))];
%             subGainsRight = [ones(size(rightSpeakers)), 0.5*ones(size(rightSpeakers))];
%             subGains = { subGainsLeft, subGainsRight };
%             subGainAdjustDB = [ -20*log10(norm(subGainsLeft)), -20*log10(norm(subGainsRight))];
%             subDelayAdjust = [ 0, 0 ];
            
            % Basic 'empty' subwoofer configuration 
            subChannels = [];
            subIndices = {  };
            subGains = { };
            subGainAdjustDB = [];
            subDelayAdjust = [];

            is2D = false;
            isInfinite = false;
    end
            
            
    numRegularSpeakers = length(speakerIndices);    
    numVirtualSpeakers = size(virtualSpeakers,1);
    usedSpeakers = [allSpeakersCart( speakerIndices,: ); virtualSpeakers];
    virtualSpeakerIndices = length(speakerIndices) + 1:numVirtualSpeakers;
    
    allChannelIndices = -ones( max( [subChannels, speakerIndices]), 1 );
    allChannelIndices( speakerIndices ) = channels;
    allChannels = [channels, zeros(numVirtualSpeakers,1)];
            
    spkCoordsNorm = usedSpeakers ./ repmat( sqrt(sum(usedSpeakers.^2, 2)), 1, 3);
    DT = delaunayTriangulation(spkCoordsNorm); % This computes a partitioning of the volume into tetrahedra
    [triplets,~] = convexHull( DT );
    
    figure(1);
    hold off;
    trisurf(triplets, DT.Points(:,1), DT.Points(:,2), DT.Points(:,3) );
    xlabel('x [m]');ylabel('y [m]');zlabel('z [m]');
    axis equal;
    labels = allSpeakerLabels( speakerIndices );
    text(DT.Points(1:numRegularSpeakers,1), DT.Points(1:numRegularSpeakers,2), DT.Points(1:numRegularSpeakers,3), labels, 'horizontal','left', 'vertical','bottom', 'FontSize', 14);
%   If the configuration is to be printed
%     figName = ['../' configName '.png'];
%     print( 1, figName, '-dpng' );
    
    %% Create the subwoofer configuration
    numSubs = length( subChannels );
    
    subSpeakerIndices = cell( numSubs, 1 );
    subSpeakerGains = cell( numSubs, 1 );
    for subIdx = 1:numSubs
        speakerIndicesList = allChannelIndices( subIndices{subIdx} ); % already  one-offset
        subIndicesStr = sprintf( ', %d', speakerIndicesList );
        subIndicesStr = subIndicesStr(3:end); % Remove the initial ', '
        subSpeakerIndices(subIdx) = { subIndicesStr };
        subGainsStr = sprintf( ', %f', subGains{subIdx} );
        subGainsStr = subGainsStr(3:end);
        subSpeakerGains(subIdx) = { subGainsStr };
    end
    
    subwooferConfig = struct( 'channels', subChannels,...
        'assignedLoudspeakers', {subSpeakerIndices},...
        'loudspeakerWeights', {subSpeakerGains} );
        
    numTotalChannels = length(channels) + numSubs;
    
    
    % Gain/delay adjustments not used at the moment
    gainDelayAdjust = false;
    if gainDelayAdjust
        gainAdjustDB = zeros(1,numTotalChannels );
        delayAdjust = zeros(1,numTotalChannels );
        % Todo: Set any adjustments to the gains and delays here.
        
        % Set the already defined gains and delays for the loudspeakers
        gainAdjustDB( subChannels ) = subGainAdjustDB;
        delayAdjust( subChannels ) = subDelayAdjust;
    end
    
    outputFileXml = ['../'  configName '.xml'];
    
    if gainDelayAdjust
        writeArrayConfigXml( outputFileXml, usedSpeakers, triplets, isInfinite, is2D, allChannels, subwooferConfig, gainAdjustDB, delayAdjust );
    else
        writeArrayConfigXml( outputFileXml, usedSpeakers, triplets, isInfinite, is2D, allChannels, subwooferConfig );
    end
end % configuration loop
