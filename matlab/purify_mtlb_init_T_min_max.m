function [T, alpha, beta, L] = purify_mtlb_init_T_min_max(imsize, FTsize, J, alpha, beta)
% A function that constructs T, needed to grid non-uniformly
% distributed fourier data using the min-max method. 
% This calculation follows the min-max method in Fessler et al, 2003.
% 
%|in
%|
%| FTsize - Array size of fft grid, fftsize/imsize=2 is good (row)
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


L = length(alpha)-1; %Labels of scaling factors

%alpha = horzcat(fliplr(conj(alpha(2:length(alpha)))), alpha); % Positive and negative scaling factors


Tinverse =  zeros(J,J,length(imsize)); 
[J1, J2] = ndgrid(1:J,1:J);
%Calculate the inverse of T
for l1  = (-L):L
    for l2 = (-L):L
        Tinverse =+ alpha(abs(l1)+1)*alpha(abs(l2)+1)* purify_mtlb_diric((J1 - J2 + beta*(l1 - l2))*pi/FTsize, imsize);
    end;
end;

T =  zeros(J,J);
%Invert Tinverse to find T
T = inv(Tinverse);

end

        