function [u, ureal] = purify_mtlb_init_min_max(imsize, fftsize, Jsize, T, alpha, beta, L)
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



% Calculating u(omega) as in Fessler paper.
u = @(omega, j) 0;
for k = 1:Jsize
    u = @(omega,j) u(omega, j) + exp(-1j*(omega - j)*(imsize - 1)/2 * 2 * pi/fftsize)*T(j,k)*sum(alpha .* diric((omega - k + beta*L)*2*pi/fftsize, imsize));
end

ureal = @(omega, j) 0;
for k = 1:Jsize
    ureal = @(omega,j) ureal(omega, j) + T(j,k)*sum(alpha .* diric((omega - k + beta*L)*2*pi/fftsize, imsize));
end

end

