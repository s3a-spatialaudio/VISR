

% Measured distance
radiusHubPlate = 2.793/2;

% Taken from a measurement of the distance between the loudspeaker covers.
correctionHubSpeakers = 0.0920;

% According to measurment of the offset differenc
correctionSparSpeakers = correctionHubSpeakers + 0.006;

radiusHubSpeakers = radiusHubPlate - correctionHubSpeakers;

radiusSparSpeakers = radiusHubPlate - correctionSparSpeakers;

b = radiusHubSpeakers * sqrt( 2/(5+sqrt(5))); % smaller part of the golden ratio
a = radiusHubSpeakers * sqrt( (5+sqrt(5))/10 ); % larger part of the golden ratio

vertexCoordsCartUnrotated = ...
    [  ...
      % x-z plane first 
       a  0  b;... % 1
      -a  0  b;... % 2
       a  0 -b;... % 3
      -a  0 -b;... % 4
       % x-y plane
       b  a  0;... % 5
      -b  a  0;... % 6
       b -a  0;... % 7
      -b -a  0;... % 8
       % y-z plane
       0  b  a;... % 9
       0 -b  a;... % 10
       0  b -a;... % 11
       0 -b -a;... % 12
    ].';

% Calculate the rotation angle around the y axis to make the plane 
normalVectorUnNorm = cross( vertexCoordsCartUnrotated(:,12) - vertexCoordsCartUnrotated(:,3), ...
    vertexCoordsCartUnrotated(:,11) - vertexCoordsCartUnrotated(:,3) );

normalVectorNorm = 1/norm(normalVectorUnNorm) * normalVectorUnNorm;

rotationAngle = acos( dot( [0, 0, -1], normalVectorNorm ));
rotationMtx = [ cos(rotationAngle), 0, -sin(rotationAngle);...
                0, 1, 0; ...
                sin(rotationAngle), 0, cos(rotationAngle) ];

vertexCoordsCart = (vertexCoordsCartUnrotated.' * rotationMtx).';
      
% [vertexAz, vertexEl, vertexRad] = cart2sph( vertexCoordsCart(1,:), vertexCoordsCart(2,:), vertexCoordsCart(3,:) );

speakerCoordsAll = [ ...
    sparSpeakerPos( vertexCoordsCart, 9, 10, radiusSparSpeakers ), ... % spk 1
    sparSpeakerPos( vertexCoordsCart, 2, 10, radiusSparSpeakers ), ... % spk 2
    sparSpeakerPos( vertexCoordsCart, 2, 9, radiusSparSpeakers ), ...  % spk 3
    vertexCoordsCart( :, 10 ), ...                                     % spk 4
    vertexCoordsCart( :, 2 ), ...                                      % spk 5
    vertexCoordsCart( :, 9 ), ...                                      % spk 6
    sparSpeakerPos( vertexCoordsCart, 1, 9, radiusSparSpeakers ), ... % spk 7
    sparSpeakerPos( vertexCoordsCart, 1, 10, radiusSparSpeakers ), ... % spk 8
    sparSpeakerPos( vertexCoordsCart, 7, 10, radiusSparSpeakers ), ... % spk 9
    sparSpeakerPos( vertexCoordsCart, 8, 10, radiusSparSpeakers ), ... % spk 10
    sparSpeakerPos( vertexCoordsCart, 2, 8, radiusSparSpeakers ), ... % spk 11
    sparSpeakerPos( vertexCoordsCart, 2, 4, radiusSparSpeakers ), ... % spk 12
    sparSpeakerPos( vertexCoordsCart, 2, 6, radiusSparSpeakers ), ... % spk 13
    sparSpeakerPos( vertexCoordsCart, 6, 9, radiusSparSpeakers ), ... % spk 14
    sparSpeakerPos( vertexCoordsCart, 5, 9, radiusSparSpeakers ), ... % spk 15
    vertexCoordsCart( :, 1  ), ...                                     % spk 16
    sparSpeakerPos( vertexCoordsCart, 1, 7, radiusSparSpeakers ), ... % spk 17
    vertexCoordsCart( :, 7 ), ...                                     % spk 18
    sparSpeakerPos( vertexCoordsCart, 7, 8, radiusSparSpeakers ), ... % spk 19
    vertexCoordsCart( :, 8  ), ...                                     % spk 20
    sparSpeakerPos( vertexCoordsCart, 8, 4, radiusSparSpeakers ), ... % spk 21
    vertexCoordsCart( :, 4 ), ...                                     % spk 22
    sparSpeakerPos( vertexCoordsCart, 4, 6, radiusSparSpeakers ), ... % spk 23
    vertexCoordsCart( :, 6 ), ...                                     % spk 24
    sparSpeakerPos( vertexCoordsCart, 6, 5, radiusSparSpeakers ), ... % spk 25
    vertexCoordsCart( :, 5 ), ...                                     % spk 26
    sparSpeakerPos( vertexCoordsCart, 1, 5, radiusSparSpeakers ), ... % spk 27
    sparSpeakerPos( vertexCoordsCart, 3, 7, radiusSparSpeakers ), ... % spk 28
    sparSpeakerPos( vertexCoordsCart, 7, 12, radiusSparSpeakers ), ... % spk 29
    sparSpeakerPos( vertexCoordsCart, 4, 12, radiusSparSpeakers ), ... % spk 30
    sparSpeakerPos( vertexCoordsCart, 4, 11, radiusSparSpeakers ), ... % spk 31
    sparSpeakerPos( vertexCoordsCart, 5, 11, radiusSparSpeakers ), ... % spk 32
    sparSpeakerPos( vertexCoordsCart, 3, 5, radiusSparSpeakers ), ... % spk 33
    sparSpeakerPos( vertexCoordsCart, 1, 3, radiusSparSpeakers ), ... % spk 34
    sparSpeakerPos( vertexCoordsCart, 8, 12, radiusSparSpeakers ), ... % spk 35
    sparSpeakerPos( vertexCoordsCart, 6, 11, radiusSparSpeakers ), ... % spk 36
    vertexCoordsCart( :, 3 ), ...                                     % spk 37
    vertexCoordsCart( :, 12 ), ...                                      % spk 38
    vertexCoordsCart( :, 11 ), ...                                      % spk 39
    sparSpeakerPos( vertexCoordsCart, 3, 12, radiusSparSpeakers ), ... % spk 40
    sparSpeakerPos( vertexCoordsCart, 11, 12, radiusSparSpeakers ), ... % spk 41
    sparSpeakerPos( vertexCoordsCart, 3, 11, radiusSparSpeakers ) ... % spk 42
    ];
    
% Define the speeakers to be used in this setup
usedSpeakerIndices = 4:33;

% Define virtual/imaginary speakers here.
virtualSpeakers = [....
    0, 0, -radiusHubSpeakers,...
    ].';

speakerCoordsReal = speakerCoordsAll( :, usedSpeakerIndices );

speakerCoords = [ speakerCoordsReal, virtualSpeakers ];

speakerCoordsNorm = speakerCoords ./ repmat( sqrt( sum( speakerCoords.^2, 1 )), 3, 1 );

numRealSpeakers = size( speakerCoordsReal, 2 );
numVirtualSpeakers = size( virtualSpeakers, 2 );
numSpeakers = size( speakerCoords, 2 );

%% Compute and display the Delaunay triangulation.
DT = delaunayTriangulation(speakerCoordsNorm.'); % This computes a partitioning of the volume into tetrahedra
[triplets,~] = convexHull( DT );
numTriplets = size( triplets, 1 );
figure(1);
hold off;
trisurf(triplets, DT.Points(:,1), DT.Points(:,2), DT.Points(:,3) );
axis equal;
xlabel('x [m]');ylabel('y [m]');zlabel('z [m]');

realChannelIndices = 0:numRealSpeakers-1;
channelIndices = [ realChannelIndices, -ones( 1, numVirtualSpeakers )];

subChannels = [ 31 ]; % zero-offset
subIndices = { realChannelIndices};  % zero-offset
subGains = {ones(size(realChannelIndices))};
subGainAdjustDB = [ -12 ];
subDelayAdjust = [0 ];

is2D = false;
isInfinite = false;

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
    numTotalChannels = max(channelIndices);
else
    numTotalChannels = max(max(channelIndices), max(subChannels+1));
end

gainAdjustDB = zeros( 1, numTotalChannels);
delayAdjust = zeros( 1, numTotalChannels);

% Todo: Set any adjustments to the gains and delays here.

% Set the already defined gains and delays for the loudspeakers
gainAdjustDB( subChannels+1 ) = subGainAdjustDB;
delayAdjust( subChannels+1 ) = subDelayAdjust;



outputFileXml = '../sphere_30channels_1subwoofer.xml';
    
writeArrayConfigXml( outputFileXml, speakerCoords.', triplets, isInfinite, is2D, channelIndices+1, subwooferConfig, gainAdjustDB, delayAdjust );

