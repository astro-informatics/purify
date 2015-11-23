function [u, ureal, r] = purify_mtlb_init_min_max(imsize, FTsize, Jsize, T, alpha, beta)
% A function that calculates a 1D u, the 1D interpolation kernel, for a given
% frequency omega_m.
%
%
%

%|in
%| imsize - imsize of dimension (scalar)
%| FTsize - fftsize of dimension (scalar)
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


L = length(alpha)-1; %Labels of scaling factors
%alpha = horzcat(fliplr(conj(alpha(2:length(alpha)))), alpha);
% Calculating u(omega) as in Fessler paper.
u = @(omega, j) 0;
r = @(omega, k) 0;
for l1 = (-L):L
    r = @(omega,k) r(omega, k) + alpha(abs(l1)+1) * purify_mtlb_diric((omega - k + beta*l1 )*pi/FTsize, imsize);
end

for k = 1:Jsize
    u = @(omega,j) u(omega, j) + exp(-1j*(omega - j)*(imsize - 1)/2 * 2 * pi/FTsize)*T(j,k)*r(omega,k);
end

ureal = @(omega, j) 0;
for k = 1:Jsize
    ureal = @(omega,j) ureal(omega, j) + T(j,k) * r(omega,k);
end

end

