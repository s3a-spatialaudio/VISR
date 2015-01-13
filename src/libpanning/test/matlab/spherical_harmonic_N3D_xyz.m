function [r] = spherical_harmonic_N3D_xyz ( l, m, x1, y1, z1 )


% x,y,z = direction norm

%% SPHERICAL_HARMONIC evaluates spherical harmonic functions.
%  Real valued set used for ambisonics
%
%  Discussion:
%
%    The spherical harmonic function Y(L,M,THETA,PHI)(X) is the
%    angular part of the solution to Laplace's equation in spherical
%    coordinates.
%
%    Y(L,M,THETA,PHI)(X) is related to the associated Legendre
%    function as follows:
%
%      Y(L,M,THETA,PHI)(X) = FACTOR * P(L,M)(cos(THETA)) * exp ( i * M * PHI )
%
%    Here, FACTOR is a normalization factor:
%
%      FACTOR = sqrt ( ( 2 * L + 1 ) * ( L - 1 )! / ( 4 * PI * ( L + M )! ) )
%
%    In Mathematica, a spherical harmonic function can be evaluated by
%
%      SphericalHarmonicY [ l, m, theta, phi ]
%
%    Note that notational tradition in physics requires that THETA
%    and PHI represent the reverse of what they would normally mean
%    in mathematical notation; that is, THETA goes up and down, and
%    PHI goes around.
%
%  Modified:
%
%    04 March 2005
%
%  Author:
%
%    John Burkardt
%
%  Reference:
%
%    Milton Abramowitz and Irene Stegun,
%    Handbook of Mathematical Functions,
%    US Department of Commerce, 1964.
%
%    Eric Weisstein,
%    CRC Concise Encyclopedia of Mathematics,
%    CRC Press, 1999.
%
%    Stephen Wolfram,
%    The Mathematica Book,
%    Fourth Edition,
%    Wolfram Media / Cambridge University Press, 1999.
%
%  Parameters:
%
%    Input, integer L, the first index of the spherical harmonic function.
%    Normally, 0 <= L.
%
%    Input, integer M, the second index of the spherical harmonic function.
%    Normally, -L <= M <= L.
%
%    Input, real THETA, the polar angle, for which
%    0 <= THETA <= PI.
%
%    Input, real PHI, the longitudinal angle, for which
%    0 <= PHI <= 2*PI.
%
%    Output, real C(1:L+1), S(1:L+1), the real and imaginary
%    parts of the functions Y(L,0:L,THETA,PHI).
%




x = x1;
y = y1;
z = z1;


% Reorient coords so that 1st order harmonics are aligned with x,y,z directions in order.
%  z = y1;
%  x = -z1;
%  y = x1;

r = sqrt(x*x + y*y + z*z);

m_abs = abs (m);
plm(1:l+1) = legendre_associated_normalized_N3D ( l, m_abs, z ) / sqrt(4*pi);

r1  = sqrt(x*x + y*y);

if (r == 0)
  cm = 1;
  sm = 0;
else
  c  = x/r1;
  s  = y/r1;
  csm  = (c+i*s)^m;
  cm = real(csm);
  sm = imag(csm);
end



% NB 

if ( m > 0 )
  r(1:l+1) = plm(1:l+1) * sqrt(2) * cm;  %cos ( m * phi );
elseif ( m < 0 )
  r(1:l+1) = plm(1:l+1) * sqrt(2) * -sm;  %sin ( -m * phi );
else
  r(1:l+1) = plm(1:l+1);
end

