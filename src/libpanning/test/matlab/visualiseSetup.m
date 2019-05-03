% Copyright (c) 2015, Andreas Franck, Institute of Sound and Vibration Research,
% University of Southampton, United Kingdom
% a.franck@soton.ac.uk

% Example script to visualise VBAP triangulations.

if false % Whether to read from the text config format (true) or the new
% XML-based configuration format.
% configFile = '../../../../config/isvr/22.1_audiolab.txt';
configFile = '../../../../config/bbc/bs2051-9+10+3-speakers-s3a.txt';

[spkPos, triplets, isInfinite, is2D, channelIndices] = readArrayConfigTxt( configFile );
else
configFileXml = '../../../../config/isvr/22.1_audiolab.xml';
[spkPos, triplets, isInfinite, is2D, channelIndices] = readArrayConfigXml( configFileXml );
end

% The current file format allows unassigned triplets (all indices -1) as a
% quick way to comment or alternate between tringulations.
% We prune the triplets here, as unassigned triplet indices make no sense in
% the visualisation.
validTriplets = all( triplets >= 1, 2 );
triplets = triplets(validTriplets,:);

% Normalise speaker positions to unit sphere.
spkPosNorm = spkPos ./ repmat( sqrt(sum(spkPos.^2, 2)), 1, 3); 

figure(1);
trisurf(triplets, spkPosNorm(:,1), spkPosNorm(:,2), spkPosNorm(:,3) );
xlabel('x [m]');ylabel('y [m]');zlabel('z [m]');
xlabel('x [m]');ylabel('y [m]');zlabel('z [m]');
axis equal;
labels = num2str((1:size(spkPosNorm,1)).','%d');
text( spkPosNorm(:,1), spkPosNorm(:,2), spkPosNorm(:,3), labels,...
    'horizontal','left', 'vertical','bottom', 'FontSize', 16, 'Color', [1 0 0]);
