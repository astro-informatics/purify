function [T, alpha, beta, L] = T_init(fftsize, imsize, J, alpha, beta)
% A function that constructs T, needed to grid non-uniformly
% distributed fourier data using the min-max method. 
% This calculation follows the min-max method in Fessler et al, 2003.
% 
%|in
%|
%| fftsize - Array size of fft grid, fftsize/imsize=2 is good (row)
%| imsize - Array of dimensions of final image (row)
%| J - Number of neighbours to use in interpolation kernel (scalar)
%| alpha - array of scaling factors, L>0 (assume the same coefficients for
%|  L<0. See Table 2 of Fessler et al, 2003. (row)
%| beta - scaling parameter. See Table 2 of Fessler et al, 2003. (scalar)
%|out
%| T - frequency independent interpolation matrix. (matrix)
%| alpha - array of scaling factors, for all +/- L (row)
%| beta - scaling parameter. (scalar)
%| L - +/- L values. (row)



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
            Tinverse(j,l,i) = sum(sum(alphaproduct .* diric((j - l + beta*(one' * L - L' * one))*2*pi/fftsize(i), imsize(i)))); 
        end;
    end;
end;

T =  zeros(J,J,length(imsize));
%Invert Tinverse to find T
for i = 1:length(imsize)
    T(:,:,i) = inv(Tinverse(:,:,i));
end

        