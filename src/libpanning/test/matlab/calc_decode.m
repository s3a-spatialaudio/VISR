
% calc_decode.m
%
% Created by Dylan Menzies on 19/12/2014
% Copyright (c) 2014 ISVR, University of Southampton. All rights reserved.
%
% Evaluate ambisonic decode matrix for a given array of ideal plane wave sources
% Decoding is based on sampling of integration of planewaves over harmonics.
% Then decode coeffs are just the harmonics evaluated at the array directions.
% This is valid for limit of many evenly spaced array directions,
% and also orthogonal arrangements - regular polyhedra, t-designs.


clear;
addpath('arrays');

N = 8; % maximum order of spherical harmonic used.
nACN = (N+1)^2;  % ACN = Ambisonic Channel Numbering

% Decode matrix, D[ ACN index ] [ array direction index ]


D = [];



% Load plane wave directions

X = [];

filename = 't-design_t8_P40.txt';
fid = fopen (filename, 'r');

while ~feof(fid)
    [val, count] = fscanf(fid, '%c', 1);

    if (val == 'c')
        [val, count] = fscanf(fid, '%d %lf %lf %lf', Inf);
        X = [X; val(2:4)'];   %! would better to write to an index position using val(1)
    else
        if (val =='%')
             fgetl(fid);
        end
    end
end
           
fclose (fid);

nPW = val(1);



% Calculate decode matrix

for j = 1:nPW

    % Spherical harmonics

    Y = [];
    for m = -N:N;
        y = spherical_harmonic_N3D_xyz(N,m,X(j,1),X(j,2),X(j,3));

        for n = abs(m):N;
            Y( n*n+n+m +1 ) = y(n +1); % reorder to ACN.
        end

    end
             
    D = [ D Y' ];

end

         
% Test: simulate field W=X=Y=1
% D(1,:) = D(1,:) + D(2,:) + D(4,:);

         
% Write decode matrix

filename = 'arrays/decode_N8_P40_t-design_t8_P40.txt';
fid = fopen (filename, 'w');
 
for j = 1:nACN
 
     for k = 1:nPW
        fprintf(fid, '%f', D(j,k));
     end
     fprintf(fid, '\n');
end
          
fclose (fid);


         
         

% Tests:
         
         
% Orthogonality:
D * D' *(4*pi / nPW);

% Harmonic function plot:
onlyplus = @(x) (sign(x)==1).*x +1;
figure
scatter3(X(:,1),X(:,2),X(:,3),onlyplus(100*D(2,:)),'red')
hold on
scatter3(X(:,1),X(:,2),X(:,3),onlyplus(-100*D(2,:)),'blue')
hold off

% Check figure plot axes orientation:
% scatter3(X(:,1),X(:,2),X(:,3),100*X(:,1),'red')
       
         
% Save image / change view.
% view(-37.5,30)  % default view (az,el)
% saveas (1,"test.eps")
         
% Save video in Octave : To make movie using imagemagic on unix command line:
% convert -delay 10 -loop 0 scatter*.png animation.gif  (scatter1.png scatter2.png ..)
  

     

         
         
         
% Load back b-format2vbap gains calculated in visr.libpanning test:
% B-format to speaker array.
 
S = [];
 
filename = 'decodeB2VBAP.txt';
fid = fopen(filename, 'r');
 
for j = 1:9
    for k = 1:6
         S(j,k) = fscanf(fid, '%f', 1);
    end
end

fclose (fid);
         
         
        
         
         
% Load coordinates for an actual speaker array.
% Plot b-format2vbap gains for individual harmonics.
         
X = [];

filename = 'octahedron.txt';
fid = fopen (filename, 'r');

while ~feof(fid)
    [val, count] = fscanf(fid, '%c', 1);

    if (val == 'c')
         [val, count] = fscanf(fid, '%d %lf %lf %lf', Inf);
         X = [X; val(2:4)'];  
    else
        if (val =='%')
            fgetl(fid);
        end
    end
end

fclose (fid);

figure
scatter3(X(:,1),X(:,2),X(:,3),onlyplus(10*S(2,:)),'red')
hold on
scatter3(X(:,1),X(:,2),X(:,3),onlyplus(-10*S(2,:)),'blue')
hold off
              

