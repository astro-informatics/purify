function [u, T, r] = u_init_1d(m, imsize, fftsize, J, T, omega_m, km, alpha, beta, L)
% A function that calculates a 1D u, the 1D interpolation kernel, for a given
% frequency omega_m.
%
%
% 


%|in
%| m - frequency index to interpolate (scalar)
%| imsize - imsize of dimension (scalar)
%| fftsize - fftsize of dimension (scalar)
%| J - Number of nearest neighbours in interpolation (scalar)
%| T - Pre-calculated interpoaltion matrix (Matrix)
%| omega_m - Array of 'digital' frequency coordinates (takes in a column)
%| km - Array of frequency coordinates on the grid (takes in a column)
%| alpha - positive and negative L scaling factors (row)
%| beta - scaling parameter (scalar)
%| L - positive and negative indicies of alpha (row)
%|
%| out
%| u - 1D interpolation kernel

r = zeros(J,1);
%Calculating r vector for given m
for j = 1:J
    r(j) = sum(sum(alpha .* diric(omega_m(m) - km(m) - j + beta*L, imsize)));
end

%calculating delta matrix
Delta = exp(1j*(omega_m(m)-(km(m) + (1:J)' )*(imsize-1)/2 * 2*pi/fftsize)); %with complex conjugate applied

u = Delta .*(T * r);