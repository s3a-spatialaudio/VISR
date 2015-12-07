function hoaSignal = encodeObjectHoa( sourceSignal, hoaOrder, objectAz, objectEl, blockSize )

if nargin < 5
    blockSize = 1;
end

if ~isvector( sourceSignal )
    error( 'The argument "sourceSignal" must be a vector.' );
end
sourceSignal = sourceSignal(:);
signalLength = length(sourceSignal);

if isscalar( objectAz) && isscalar( objectEl )
    blockSize = signalLength;
end

% This discards an incomplete block at the end.
numBlocks = floor( signalLength / blockSize );
% optimisation: if both are scalar, compute the whole signal in one go.

interpolationRamp = [0:blockSize-1].'/blockSize;

numHoaSignals = (hoaOrder+1)^2;

hoaSignal = zeros( signalLength, numHoaSignals );

for blockIdx = 1:numBlocks
    outputIndices = [(blockIdx-1)*blockSize+1:blockIdx*blockSize];
    
    if blockIdx < length( objectAz )
        currAz = objectAz( blockIdx );
        nextAz = objectAz( blockIdx+1 );
    else
        currAz = objectAz( end );
        nextAz = objectAz( end );
    end
    if blockIdx < length( objectEl )
        currEl = objectEl( blockIdx );
        nextEl = objectEl( blockIdx+1 );
    else
        currEl = objectEl( end );
        nextEl = objectEl( end );
    end
    for degree = -hoaOrder:hoaOrder
        yCurr = spherical_harmonic_N3D( hoaOrder, degree, pi/2 - currEl, currAz );
        yNext = spherical_harmonic_N3D( hoaOrder, degree, pi/2 - nextEl, nextAz );
        
        for order = abs(degree):hoaOrder
            acnIdx = acnIndex( order, degree );
            gain = yCurr(order+1) + (yNext(order+1)-yCurr(order+1))*interpolationRamp;
            compSig = gain.*sourceSignal(outputIndices);
            hoaSignal( outputIndices, acnIdx ) = compSig;
        end
    end
end % for blockIdx = 1:numBlocks

end % function

% Local function to translate the spherical harmonics order and degree into
% the ACN signal index (1-offset as we are using matlab
function i = acnIndex( n, m )
    i = n*n+m+n+1;
end