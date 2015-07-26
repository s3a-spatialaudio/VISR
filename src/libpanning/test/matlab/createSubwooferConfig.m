% Copyright (c) 2015, Andreas Franck, Institute of Sound and Vibration Research,
% University of Southampton, United Kingdom
% a.franck@soton.ac.uk

% Example script for writing a array configuration file including the
% subwoofer.


%% First, load an existing pure array configuration
if false % Whether to read from the text config format (true) or the new
% XML-based configuration format.
configFile = '../../../../config/isvr/22.1_audiolab.txt';
% configFile = '../../../../config/bbc/bs2051-9+10+3-speakers-s3a.txt';

[spkPos, triplets, isInfinite, is2D, channelIndices] = readArrayConfigTxt( configFile );
else
configFileXml = '../../../../config/isvr/22.1_audiolab.xml';
[spkPos, triplets, isInfinite, is2D, channelIndices] = readArrayConfigXml( configFileXml );
end

%% Provide a subwoofer configuration
numSubs = 2;
subOutputChannels = [40 41];

% Transmit strings rather than arrays to allow for more concise
% representations.
subSpeakerIndices = cell(numSubs,1);
subSpeakerIndices(1) = {'1:11'};
subSpeakerIndices(2) = {'12:22'};

w1 = hamming( 11 );
w2 = ones( 1, 11 );

w1str = sprintf( ', %f', w1 );w1str = w1str(3:end);
w2str = sprintf( ', %f', w2 );w2str = w2str(3:end);

subSpeakerGains = cell( numSubs,1);
% Note: We need row vectors.
subSpeakerGains(1) = {w1str};
subSpeakerGains(2) = {w2str};

if ~isvector(subOutputChannels) || length(subOutputChannels) ~= numSubs
    error( 'The length of the vector "subOutputChannels" must match the number of subwoofers.' )
end

subwooferConfig = struct( 'channels', subOutputChannels,...
    'assignedLoudspeakers', {subSpeakerIndices},...
    'loudspeakerWeights', {subSpeakerGains} );

outputFileXml = '../../../../config/isvr/22.1_audiolab_subwoofers.xml';

% function  [outputString] = writeArrayConfigXml( fileName, spkPos, triplets, isInfinite, is2D, channelIndices, subwooferConfig )

writeArrayConfigXml( outputFileXml, spkPos, triplets, isInfinite, is2D, channelIndices, subwooferConfig );

