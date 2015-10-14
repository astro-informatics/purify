function [T, alpha, beta, L] = T_init(K, imsize, J, alpha, beta)
% A function that constructs T, needed to grid non-uniformly
% distributed fourier data using the min-max method. 
% This calculation follows the min-max method in Fessler et al, 2003.
% 
%|in
%|
%| K - oversampling factor fftsize/imsize, 2 is a good/best choice
%| imsize - Array of dimensions of final image
%| J - Number of neighbours to use in interpolation kernel
%| alpha - array of scaling factors, L>0 (assume the same coefficients for
%|  L<0. See Table 2 of Fessler et al, 2003.
%| beta - scaling parameter. See Table 2 of Fessler et al, 2003.
%|out
%| T - frequency independent interpolation matrix.
%| alpha - array of scaling factors, for all +/- L
%| beta - scaling parameter.
%| L - +/- L values.

% Defines oversampling of FFT by factor of 2 (K/N = 2)
fftsize = imsize*K;


alpha = horzcat(fliplr(alpha(2:length(alpha))), alpha); % Positive and negative scaling factors

L = 1:length(alpha); %Labels of scaling factors
L = L - (length(alpha)+1)/2;


alphaproduct = alpha' * alpha; % Outerproduct for summation later


one = L*0 + 1; % ones for outer product in summation

Tinverse =  zeros(J,J,length(imsize)); 

%Calculate the inverse of T
for i = 1:length(imsize)
    for j = 1:J
        for l = 1:J
            % The matlab diric function takes in different units from the
            % Fessler paper.
            Tinverse(j,l,i) = sum(sum(alphaproduct * diric((j - l + beta*(one' * L - L' * one))*2*pi/fftsize(i), imsize(i)))); 
        end;
    end;
end;

T =  zeros(J,J,length(imsize));
%Invert Tinverse to find T
for i = 1:length(imsize)
    T(:,:,i) = inv(Tinverse(:,:,i));
end

        