function value = d_factorial ( n )

%% D_FACTORIAL computes the factorial N!
%
%  Formula:
%
%    FACTORIAL( N ) = PRODUCT ( 1 <= I <= N ) I
%
%  Modified:
%
%    25 July 2004
%
%  Author:
%
%    John Burkardt
%
%  Parameters:
%
%    Input, integer N, the argument of the factorial function.
%    If N is less than 1, the value is returned as 1.
%
%    Output, real VALUE, the factorial of N.
%
  value = 1.0;

  for i = 1 : n
    value = value * i;
  end
