% Example of 1d gridding.


imsize = 1024; % output image size

fftsize = 2 *imsize; % output fftsize

M = 1000; % Number of frequencies to grid

omega_m = ((-M/2):0.5:M/2)'; % Frequencies to grid

data = omega_m * 0 + 1; % Phase and amplitude

J = 10; % Number of neighbours to interpolate

alpha = [1]; % Scaling factors

beta = 0; % Scaling parameter

grid = generate_fftgrid_1d(data, omega_m, imsize, fftsize, J, alpha, beta); % data put onto a grid

x=fftshift(ifft(grid)); % Inverse FFT of data

plot(1:length(x),abs(x)); % Plot of data

plot(1:length(x),real(x)); % Plot of data

plot(1:length(x),imag(x)); % Plot of data