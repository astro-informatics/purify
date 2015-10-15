%Tests generating the interpolation kernel in 1D, and creates plots
%for each frequency omega_m (digital).


alpha = [1, -0.57, 0.185]; % Scaling factors
beta = 0.43; % Scaling parameter
J=10; % Number of nearest neighbours
imsize= [1024]; % Image size
fftsize = [2048]; % FFT grid size
omega_m = ((-4):1:4)'; % Ungridded frequencies.



% Generate T from scaling variables alpha and beta.
[T, alpha, beta, L] = T_init(fftsize ,imsize, J, alpha, beta);

% Match omega_m values to km (on the grid), and offset them by J/2
km = omega_to_k(fftsize, omega_m, J);

M = length(omega_m); % Number of interpolation kernels to calculate

% Loop to generate and plot the interpolation kernels 
for m = 1:length(omega_m)
    subplot(ceil(sqrt(M)), ceil(sqrt(M)),m)
    [u, T, r] = u_init_1d(m, imsize, fftsize, J, T, omega_m, km, alpha, beta, L);
    plot(1:J,abs(u))
    t = sprintf('\\omega_m = %.2f',omega_m(m));
    title(t)
    xlabel('J') % x-axis label
    ylabel('u_J') % y-axis label
end