function [y] = uniformlyPartitionedConvolution( inputSignal, h, bs )
% uniformlyPartitionedConvolution
% Compute the (noncyclic) convolution of an input signal with a filter
% impulse response by a uniform partitioned convolution.

% Parameters:
% inputSignal: signal to be convolved. must be a vector (i.e. a scalar
% vector) If the length of the signal is not a multiple of the block size,
% it is zero-padded at the end to the next integral multiple of the block
% size.
% h: the impulse response to convolve the input signal with. Must be an
% integral of the block size.
% bs: The block size of the partitioned convolution algorithm. It controls
% the complexity and the latency of the algorithm (if implemented in a
% realtime context.) For the targeted practical applications, the block
% size must be a power of 2, so we use this restriction here, too

if nargin ~= 3
    error( 'Usage: uniformPartitionedConvolution( inputSignal, filter, blockSize )' );
end

if rem( log2( bs ), 1 ) ~= 0
    error( 'The block size must be a power of 2' );
end

if ~isvector(inputSignal)
    error( 'inputSignal must be a vector. At the moment, only scalar convolution is supported' );
end

if ~isvector(h)
    error( 'h must be a vector. At the moment, only scalar convolution is supported' );
end

inputSignal = inputSignal(:);
h =  h(:);

% Argument check and adjustment
if mod( length(h), bs ) ~= 0
    error( 'The length of the impulse response must be an integer multiple of the block size' );
end;

numInputBlocks  = ceil( length( inputSignal )/bs );
inputLength = numInputBlocks * bs;

% prepend bs zeros to enable the startup of the filter.
paddedInput = [ zeros(bs,1); inputSignal(:); zeros( inputLength - length(inputSignal), 1 ) ];

numFilterBlocks = length(h)/bs;

% Partition into filter blocks and append zero-padding: Eech row contains a
% partition of size bs followed by bs zeros
% Note: To be exact, bs-1 zeros would be sufficient.
hBlocked = [reshape(h, bs, numFilterBlocks )', zeros(numFilterBlocks, bs)];

% Transform the zero-padded filters into the frequency domain separately.
HBlocked = fft( hBlocked, [], 2 );

% Allocate a frequency delay line for intermediate results. Each of the
% numFilterBlocks rows contains a frequency-doamin representation of size
% bs*2 complex values.
FDL = zeros( numFilterBlocks, bs*2 );

% Allocate the output vector.
% The output size equals the input size padded to the next multiple of the
% block size. If the length should match exactly, prune the output
% afterwards.
y = zeros(inputLength, 1 );

for runIdx = 0:numInputBlocks-1
    
    % Calculate a FFT of an input block of size 2*bs (that is, for each
    % input block, the current samples and the preceding block is taken ->
    % Overlap)
    xBlock = paddedInput(runIdx*bs+1: (runIdx+2)*bs );
    xTransformed = fft( xBlock' );
    
    % multiply 
    FDL = FDL + repmat( xTransformed, numFilterBlocks, 1 ) .* HBlocked;
    
    yPadded  = ifft( FDL(1,:) );
    
    % Discard the first half of the inverse FFT -> this is the second part
    % of the overlap-and-save (synonymously, overlap-and-discard)
    % algorithm. See http://en.wikipedia.org/wiki/Overlap-save_method
    y( runIdx*bs+1 : (runIdx+1)*bs ) = yPadded( bs+1:2*bs);
    
    % shift the frequency-domain delay line one row up, filling the last
    % row with zeros.
    % Unfortunately, there is no ordinary 'shift' operator in matlab that appends
    % zeros, so we do it by hand.
    FDL = [ FDL(2:numFilterBlocks, : ); zeros(1,bs*2 ) ];
end

end