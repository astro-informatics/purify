% Example of 1d gridding.

clear

imsize = 1024; % output image size

fftsize = 2 * imsize; % output fftsize

M = 500; % Number of frequencies to grid

omega_m = normrnd(0,fftsize*0.05, [M,1]); % Frequencies to grid

data = omega_m * 0 + 1; % Phase and amplitude

J = 4; % Number of neighbours to interpolate

alpha = [1, -0.5319, 0.1522, -0.0199]; % Scaling factors

beta = 0.6339; % Scaling parameter

grid = generate_fftgrid_1d(data, omega_m, imsize, fftsize, J, alpha, beta); % data put onto a grid

x=fftshift(ifft(grid)); % Inverse FFT of data

figure;
plot(1:length(x),abs(x)); % Plot of data absolute value
figure;
plot(1:length(x),real(x)); % Plot of data real part
figure;
plot(1:length(x),imag(x)); % Plot of data imaginary part

spy(abs(grid)) % Showing how sparse the grid is