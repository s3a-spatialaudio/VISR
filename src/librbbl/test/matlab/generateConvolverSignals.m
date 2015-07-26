
signalLength = 131072;

filterLength = 1024;

numSignals = 4;

filters = 2*(rand(numSignals, filterLength )-0.5)/sqrt(filterLength);

input = 2*(rand(numSignals, signalLength )-0.5);

output = zeros( numSignals, signalLength );

channelGains = [ 1 1 0 0 ];

for chIdx = 1:numSignals
    y = conv( filters( chIdx, : ), input(chIdx,:), 'full' );
    output( chIdx, : ) = channelGains(chIdx) * y(1:signalLength);
end

audiowrite( '../signals/filters_4ch.wav', filters.', 48e3, 'BitsPerSample', 32 );
audiowrite( '../signals/input_4ch.wav', input.', 48e3, 'BitsPerSample', 32 );
audiowrite( '../signals/output_4ch.wav', output.', 48e3, 'BitsPerSample', 32 );
