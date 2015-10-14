function km = omega_to_k(fftsize, omega_m, J)
% A function that maps omega_m to k_m, where omega_m is a `digital' frequency
% off the grid, and gamma * k_m is on the grid.
% This calculation follows the min-max method in Fessler et al, 2003.
% The grid frequencies are also shifted by J/2 for interpolation.


%|in
%|
%| fftsize - Array size of fft dimensions
%| omega_m - Array of 'digital' frequency coordinates (takes in a column
%| for each dimension
%| J - Number of nearest neighbours in interpolation
%| out
%| km - Array of gridded frequency coordinates


km = omega_m * nan;

%Calculating gridded frequency coordinates k_m, which have been shifted for
%interpolation by J/2.
if mod(J, 2) == 0
    for l = 1:length(fftsize) 
        kRange = (0:fftsize(l)) - fftsize(l)/2;
        for m = 1: length(omega_m(:,l))
            temp = omega_m(m, l) - kRange;
            temp(temp<0)=nan;
            [~, minI] = min(temp);
            km(m,l) = minI;
        end
    end
    km = km - J/2;
else
    for l = 1:length(fftsize)
        kRange = (0:fftsize(l)) - fftsize(l)/2;
        for m = 1: length(omega_m(:,l))
            temp = abs(omega_m(m, l) - kRange);
            temp(temp<0)=nan;
            [~, minI] = min(temp);
            km(m,l) = minI;
        end
    end
    km = km - (J+1)/2;
end