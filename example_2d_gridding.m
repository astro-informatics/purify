% Example of 2d gridding.

clear

vis = textread('../../test_dev/1637-77.vis');

U = vis(:,1);
V = vis(:,2);
data = vis(:,3) + 1j*vis(:,4);
U = [U; -U];
V = [V; -V];

data = [data; conj(data)];
U = U/sqrt(max(U.^2+V.^2));
V = V/sqrt(max(U.^2+V.^2));
omega_m = [U, V]*128;


imsize = [1024, 1024]; % output image size

fftsize = 2 * imsize; % output fftsize
%data = data*0 + 1;
%M = 5000; % Number of frequencies to grid

%omega_m = normrnd(0,fftsize(1)*0.05, M, 2); % Frequencies to grid

%data = omega_m * 0 + 2*M; % Phase and amplitude1

J = 6; % Number of neighbours to interpolate

alpha = [1]; % Scaling factors

beta = 0; % Scaling parameter

grid = generate_fftgrid_2d(data, omega_m, imsize, fftsize, J, alpha, beta); % data put onto a grid

x = ifftshift(fft2(fftshift(grid), imsize(1),imsize(2))); % Inverse FFT of data
x = transpose(x);
%fitswrite(x, 'test.fits');
figure;
x = x/max(max(abs(x)))*100;
x = rot90(x,2);
image(abs(x)) % Plot of data absolute value
figure;
spy(abs(grid)) % Showing how sparse the grid is
fitswrite(real(x),'real1637.fits')
fitswrite(abs(x),'abs1637.fits')
fitswrite(imag(x),'imag1637.fits')
fitswrite(real(grid),'realgrid1637.fits')
fitswrite(imag(grid),'imaggrid1637.fits')