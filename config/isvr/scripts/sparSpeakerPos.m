% Copyright ISVR 2015 - All rights reserved
% Author: Andreas Franck a.franck@soton.ac.uk
% Calculate the position of the 'spar speakers' in the small ISVR sphere.
function [pos] = sparSpeakerPos( hubSpeakerMtx, idx1, idx2, sparRadius )

posUnNorm = 0.5*hubSpeakerMtx(:,idx1) + 0.5*hubSpeakerMtx(:,idx2);

pos = sparRadius/norm(posUnNorm) * posUnNorm;

end