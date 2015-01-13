
clear;
rand('state',1000); % Set seed - can use this to optimize thinning of CIPIC.
load('hrir_final.mat');

lmax = 15; % maximum order of spherical harmonic used.
rm = 1.2; % Distance of source in metres. 
% Careful!! If shifted hrirs used, this is relative to the ear.
% Also true hrtfs need 1/r factor relative to ear centre.

nsamps = 200; % in each impulse.
zp = nsamps*15; % Zero padding to boost frequency resolution.
nf = (16*nsamps)/2; % Number of frequency samples.
sr = 44100; % Sample rate.
ss = 340; % Speed of sound.
fmax = sr/2;  % For printing.

% Test hrir
vecm_n = 1002; %1002 %502   % Source vector. xx01 - left side   xx25 - right side.
[vecm(1),vecm(2),vecm(3)] = cpic_vec(vecm_n);
% vecm = [0,0,-1.0]; % Direct left;
 vecm = [0,0,1.0]; % Direct right;

[phim, elv, r] = cart2sph( vecm(1), vecm(2), vecm(3));       
thetam = pi/2-elv;
hrirm_l = cpic_hrir_shift(vecm_n, hrir_l, hrir_r)';  % Measured HRIR for comparison with synth.
hrirm1_l = cpic_hrir_shift(vecm_n+1, hrir_l, hrir_r)'; % Neighbours to test change
hrirm2_l = cpic_hrir_shift(vecm_n-1, hrir_l, hrir_r)';



disp('Load CIPIC..');
% CIPIC
nvecs = 1250; %1249; %floor((lmax+1)*(lmax+1)*1.6); %+200
if (nvecs > 1249) nvecs = 1249; end   % Source will be reserved.
div = 1250/nvecs;
vecs = [];
veci = [];
taken = zeros(1,1250);
taken(vecm_n) = 1; % Reserve for test hrir.
for j = 1:nvecs
% Choose method:
    jj = j;
%    if (j == vecm_n) j = j+1; end;  jj = j;  
%    jj = floor(j*div);
%    t = 1; while(t == 1) jj=ceil(rand*1250); t = taken(jj); end; taken(jj) = 1; %Random.
    veci = [veci jj];
    [x y z] = cpic_vec(jj);  % thin out vecs.
    vecs(j,:) = [x y z];
    [l r] = cpic_hrir_shift(jj, hrir_l, hrir_r);
    hrirs_l(j,:) = l;
    hrirs_r(j,:) = r;
end



disp('Calc D..');
% Calc decoding matrix from vecs.
C = []; % 're-encoding matrix'.
for ivecs = 1 : nvecs
    [phi, elv, r] = cart2sph( vecs(ivecs,1), vecs(ivecs,2), vecs(ivecs,3) );        
    theta = pi/2-elv;
%   phi = phi + rand()*0.001;  % Check effect of angle error on HRIRs.

%     yy = [];
%     for m = -lmax:lmax;
%         y = spherical_harmonic_N3D(lmax,m,theta,phi);
%         yy = [yy y(abs(m)+1:lmax+1)];
%     end
    
    Y = [];  % Temporary 2 dim storage for spherical coefs.
    for m = -lmax:lmax;
        y = spherical_harmonic_N3D(lmax,m,theta,phi);
        Y = [Y y.'];   % (l,m) matrix
    end
    
    yy = [];   
    for l = 1:lmax+1
        yy = [yy Y(l, lmax-l+2:lmax+l)];  % Create linear order by l. Makes later calculation easier.
    end
    
    C = [C yy.'];
end
D = pinv(C);  % Decoding matrix.

max(max(C*D-eye((lmax+1)*(lmax+1))))  % Measure of decoding performance




disp('Calc B for source..');
% Calc spherical harmonics.
Ym = [];
for m = -lmax:lmax;
    y = spherical_harmonic_N3D(lmax,m,thetam,phim);
    Ym = [Ym y.'];   % (l,m) matrix
end

yym = [];   
for l = 1:lmax+1
    yym = [yym Ym(l, lmax-l+2:lmax+l)];  % Create linear order by l. For test calc.
end




%Fm = dist_filt(lmax, rm);
%hm = sqrt(pi/(2*rm)) * ( besselj(0.5:lmax+.5,rm) - i * bessely(0.5:lmax+.5,rm) );



disp('Calc HRIRs..');
%Calculate HRIRs, components of the final hrirs_l and_r.
hrirSplit_l = [];  % HRIR by order
hrirSplit_r = []; 
hrir_l = 0;
hrir_r = 0;
hrtf_l = 0;
hrtf_r = 0;

lm = 1; % Count thro l,m coeffs.
S = zeros(1,nvecs); % Virtual speaker coeffs.
for l=1:lmax+1
    hl = 0;
    hr = 0;
    for j=1:nvecs
        DYm = sum( D(j, lm:lm+2*l-2) .* Ym(l, lmax-l+2:lmax+l) );
        hl = hl + hrirs_l(j,:) * DYm;
        hr = hr + hrirs_r(j,:) * DYm;
        S(j) = S(j) + DYm;
    end
    lm = lm + 2*l-1;
    hrirSplit_l = [hrirSplit_l; hl];
        H = fft([hl zeros(1,zp)]);
 %       plot(0:fmax/nf:fmax, 10*log10(abs(H(1:nf+1))), 'c' );
        hold on;
    hrirSplit_r = [hrirSplit_r; hl];
    
    hrir_l = hrir_l + hl;
    hrir_r = hrir_r + hr;
    
    % Distance function, only acting on hrtf_l, hrtf_r
    F = [];
    for f = 0.001:fmax/nf:fmax-fmax/nf+0.001;
        d = dist_filt(l-1, rm * 2*pi*f/ss);
        d = d(l);
       %d=1;
        if (abs(d)>1000) d = d/abs(d)*1000; end;
        F = [F d];
    end
    
    %loglog(abs(F(1:nf))); hold on;

    F = [F zeros(1,nf)]; % Second half isn't used.
    
    hrtf_l = hrtf_l + fft([hl zeros(1,zp)]) .* F;
    hrtf_r = hrtf_r + fft([hr zeros(1,zp)]) .* F;
end

% Calc farfield hrtf in a different way using S, and check.
hl = 0;
for j = 1:nvecs  hl = hl + S(j) * hrirs_l(j,:);  end
max(hl - hrir_l)
max(C*S.'-yym.')  % compare target coeffs with re-encoded coeffs.
max(max(D))


%plot(0:fmax/nf:fmax, 10*log10(abs(hrtf_l(1:nf+1))), 'r' );
plot(0:fmax/nf:fmax, 10*log10(abs(hrtf_l(1:nf+1)./rm)), 'r' ); % 1/r factor included!
s

H = fft([hrirm_l zeros(1,zp)]);
plot(0:fmax/nf:fmax, 10*log10(abs(H(1:nf+1))), 'b' );
hold on;

H = fft([hrir_l zeros(1,zp)]);
plot(0:fmax/nf:fmax, 10*log10(abs(H(1:nf+1))), 'r' );
hold on
s

plot(hrirm_l, 'b'); hold on; plot(hrir_l, 'r');
s





H = fft([hrirm1_l zeros(1,zp)]);
plot(0:fmax/nf:fmax, 10*log10(abs(H(1:nf+1))), 'g' );
hold on;

H = fft([hrirm2_l zeros(1,zp)]);
plot(0:fmax/nf:fmax, 10*log10(abs(H(1:nf+1))), 'g' );
hold on;


