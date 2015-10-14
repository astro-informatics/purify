function u = u_init(m, N, K, J, T, omega_m, km, alpha, beta, L)
% A function that calculates a 1D u, the 1D interpolation kernel, for a given
% frequency omega_m.
%
%
% 


%|in
%| m - frequency index to interpolate (scalar)
%| N - imsize of dimension (scalar)
%| K - fftsize of dimension (scalar)
%| J - Number of nearest neighbours in interpolation (scalar)
%| T - Pre-calculated interpoaltion matrix (Matrix)
%| omega_m - Array of 'digital' frequency coordinates (takes in a column)
%| km - Array of frequency coordinates on the grid (takes in a column)
%| alpha - positive and negative L scaling factors
%| beta - scaling parameter
%| L - positive and negative indicies of alpha 
%|
%| out
%| u - 1D interpolation kernel

r = zeros(J)';
%Calculating r vector for given m
for j = 1:J
    r(j) = sum(alpha * diric(omega_m(m)-km(m) -j +beta*L, N));
end

%calculating delta matrix
Delta = exp(1j*(omega_m(m)-(km(m) + (1:J)' )*(N-1)/2 * 2*pi/K); %with complex conjugate applied

u = Delta*(T * r);