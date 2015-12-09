clc
clear;

%%
addpath src/
addpath data/
addpath ../../sopt/matlab
addpath ../../sopt/matlab/misc
%Linking Nufft
addpath ../../../Software/irt/nufft
addpath ../../../Software/irt/utilities
addpath ../../../Software/irt/systems

%stringname='ppdisk672_GHz_50pc';
stringname='cluster';
%stringname='g41.1-0.3.b';
%stringname='HCO+_4-3_60';
%stringname='3c288';
%stringname='30dor';
%stringname='CYGCBEST';
%stringname='M87ABABCPCONV6';
%stringname='M51HA'; 
%stringname='M31';
%stringname='eta-carinae_ref-img';

stringname1=[stringname '.fits'];
%info = fitsinfo(stringname1)


%% Load an image
data = fitsread(stringname1);
data=flipud(data);
im=im2double(data);
im=im/max(max(im));
im(im<0)=0;


%% Parameters
% Noise level (on the measurements)
input_snr = 30;



[Ny1,Nx1]=size(im);
Of=2;
Nx2 = Of*Nx1;
Ny2 = Of*Ny1;

%Load coverage

load AMI_02

u1 = Data(:,2)*pi/2000;
v1 = Data(:,3)*pi/2000;

u1 = [u1; -u1];
v1 = [v1; -v1];
num_meas=length(u1);
clear Data

%% Compute nufft parameters

    
Ju = 4;
Jv = 4;
Fa = 2;
% %tau1 = (Fa/Nx1)*2*pi;
% %tau2 = (Fa/Ny1)*2*pi;
% 
% tau1 = (Fa/Nx1);
% tau2 = (Fa/Ny1);


%Initialize nufft parameters
fprintf('Initializing the NUFFT operator\n\n');
tstart1=tic;
st = nufft_init([v1 u1],[Ny1 Nx1],[Jv Ju],[Ny2 Nx2], [Ny1/2 Nx1/2], 'kaiser');
tend1=toc(tstart1);
fprintf('Time for the initialization: %e\n\n', tend1);


%Operator functions

A = @(x) nufft(x, st);
At = @(x) nufft_adj(x, st);

% Select p% of Fourier coefficients
y0 = A(im);
% Add Gaussian i.i.d. noise
%sigma_noise = 10^(-input_snr/20)*std(im(:));
sigma_noise = 10^(-input_snr/20)*(norm(y0)/sqrt(num_meas));
noise = (randn(size(y0)) + 1i*randn(size(y0)))*sigma_noise/sqrt(2);
y1 = y0 + noise;

%Size of the problem
M = length(y1);
N = Ny1*Nx1;

  
%Maximum eigenvalue of operato A^TA
eval = pow_method(A, At, [Ny1,Nx1], 1e-4, 100, 1);

%Bound for the L2 residual
epsilon = sqrt(M + 2*sqrt(M))*sigma_noise;

%Dirty image
dirty = At(y1);
dirty1 = 2*real(dirty)/eval;

tau1 = (Fa/Nx2);
tau2 = (Fa/Ny2);

st = purify_mtlb_init_ftgrid([u1 v1],Ny1,Nx1,Jv,Ju,tau2,tau1,Of,Of);


%Define operators
A = @(x) purify_mtlb_ftgridfwd(x,st);

At = @(y) purify_mtlb_ftgridadj(y,st);


% Select p% of Fourier coefficients
y0 = A(im);
% Add Gaussian i.i.d. noise
%sigma_noise = 10^(-input_snr/20)*std(im(:));
sigma_noise = 10^(-input_snr/20)*(norm(y0)/sqrt(num_meas));
noise = (randn(size(y0)) + 1i*randn(size(y0)))*sigma_noise/sqrt(2);
y2 = y0 + noise;

%Size of the problem
M = length(y2);
N = Ny1*Nx1;

  
%Maximum eigenvalue of operato A^TA
eval = pow_method(A, At, [Ny1,Nx1], 1e-4, 100, 1);

%Bound for the L2 residual
epsilon = sqrt(M + 2*sqrt(M))*sigma_noise;

%Dirty image
dirty = At(y2);
dirty2 = 2*real(dirty)/eval;

st = purify_mtlb_init_nufft([v1, u1], [Ny1, Nx1], Of, 'gauss', [Jv, Ju]);

%Define operators
A = @(x) purify_mtlb_degrid(x,st);

At = @(y) purify_mtlb_grid(y,st);


% Select p% of Fourier coefficients
y0 = A(im);
% Add Gaussian i.i.d. noise
%sigma_noise = 10^(-input_snr/20)*std(im(:));
sigma_noise = 10^(-input_snr/20)*(norm(y0)/sqrt(num_meas));
noise = (randn(size(y0)) + 1i*randn(size(y0)))*sigma_noise/sqrt(2);
y3 = y0 + noise;

%Size of the problem
M = length(y3);
N = Ny1*Nx1;

  
%Maximum eigenvalue of operato A^TA
eval = pow_method(A, At, [Ny1,Nx1], 1e-4, 100, 1);

%Bound for the L2 residual/
epsilon = sqrt(M + 2*sqrt(M))*sigma_noise;

%Dirty image
dirty = At(y3);
dirty3 = 2*real(dirty)/eval;


figure; image(real(dirty1)/max(max(real(dirty1)))*100);
figure; image(real(dirty2)/max(max(real(dirty2)))*100);
figure; image(real(dirty3)/max(max(real(dirty3)))*100);

figure;image(abs(fftshift(fft2(dirty1)))/max(max(abs(fftshift(fft2(dirty1)))))*100);
figure;image(abs(fftshift(fft2(dirty2)))/max(max(abs(fftshift(fft2(dirty2)))))*100);
figure;image(abs(fftshift(fft2(dirty3)))/max(max(abs(fftshift(fft2(dirty3)))))*100);

realname = sprintf('dirty1.fits');
fitswrite(real(dirty1)/max(max(real(dirty1))), realname)
realname = sprintf('dirty2.fits');
fitswrite(real(dirty2)/max(max(real(dirty2))), realname)
realname = sprintf('dirty3.fits');
fitswrite(real(dirty3)/max(max(real(dirty3))), realname)