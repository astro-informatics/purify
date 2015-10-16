function [u, T, r, D] = u_init_1d(imsize, fftsize, J, T, omega_m, km, alpha, beta, L)
% A function that calculates a 1D u, the 1D interpolation kernel, for a given
% frequency omega_m.
%
%
% 

%|in
%| imsize - imsize of dimension (scalar)
%| fftsize - fftsize of dimension (scalar)
%| J - Number of nearest neighbours in interpolation (scalar)
%| T - Pre-calculated interpoaltion matrix (Matrix)
%| omega_m - Array of 'digital' frequency coordinates (scalar)
%| km - Array of frequency coordinates on the grid (scalar)
%| alpha - positive and negative L scaling factors (row)
%| beta - scaling parameter (scalar)
%| L - positive and negative indicies of alpha (row)
%|
%| out
%| u - 1D interpolation kernel

r = zeros(J,1);
% Calculating r(omega) as in Fessler paper.
for j = 1:J
    r(j) = sum(sum(alpha .* diric((omega_m - km - j + beta*L)*2*pi/fftsize, imsize)));
end

%calculating delta matrix
D = exp(1j*(omega_m-km + (1:J)' )*(imsize-1)/2 * 2 * pi/fftsize); %with complex conjugate applied

u = D .*(T * r);