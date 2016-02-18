clear
addpath ../data/

%Linking Sopt
addpath ../../sopt/matlab/test_images/
addpath ../../sopt/matlab/misc/
addpath ../../sopt/matlab/prox_operators/
addpath ../../sopt/matlab

stringname='M31.fits';

data = fitsread(stringname);
data=flipud(data);
im=im2double(data);

%% Parameters
input_snr = 40; % Noise level (on the measurements)

p=0.5;
%d=0.1;
% Mask
%mask = rand(size(im)) < p; ind = find(mask==1);
mask=vdsmask(size(im,1),size(im,2),p);
ind = find(mask==1);
% Masking matrix (sparse matrix in matlab)
Ma = sparse(1:numel(ind), ind, ones(numel(ind), 1), numel(ind), numel(im));

N = numel(im);
M = numel(ind);

A = @(x) Ma*reshape(fft2(x)/sqrt(N), N, 1);
At = @(x) ifft2(reshape(Ma'*x(:), size(im))*sqrt(N));

y0 = A(im);
    
% Add Gaussian i.i.d. noise
sigma_noise = 10^(-input_snr/20)*norm(im(:))/sqrt(N);
noise = (randn(size(y0)) + 1i*randn(size(y0)))*sigma_noise/sqrt(2);
y = y0 + noise;

epsilon = sqrt(M + 2*sqrt(M))*sigma_noise;

%Dirty image
dirty = At(y);
dirty1 = 2*real(dirty);


%% Sparsity operator definition

%Wavelets parameters
nlevel=8;
dwtmode('per');

% Sparsity operator for SARA

[C1,S1]=wavedec2(dirty1,nlevel,'db1'); 
ncoef1=length(C1);
[C2,S2]=wavedec2(dirty1,nlevel,'db2'); 
ncoef2=length(C2);
[C3,S3]=wavedec2(dirty1,nlevel,'db3'); 
ncoef3=length(C3);
[C4,S4]=wavedec2(dirty1,nlevel,'db4'); 
ncoef4=length(C4);
[C5,S5]=wavedec2(dirty1,nlevel,'db5'); 
ncoef5=length(C5);
[C6,S6]=wavedec2(dirty1,nlevel,'db6'); 
ncoef6=length(C6);
[C7,S7]=wavedec2(dirty1,nlevel,'db7'); 
ncoef7=length(C7);
[C8,S8]=wavedec2(dirty1,nlevel,'db8'); 
ncoef8=length(C8);

clear C1 C2 C3 C4 C5 C6 C7 C8


Psit = @(x) [wavedec2(x,nlevel,'db1')'; wavedec2(x,nlevel,'db2')';...
    wavedec2(x,nlevel,'db3')';wavedec2(x,nlevel,'db4')';...
    wavedec2(x,nlevel,'db5')'; wavedec2(x,nlevel,'db6')';...
    wavedec2(x,nlevel,'db7')'; wavedec2(x,nlevel,'db8')'; x(:)]/sqrt(9); 
Psi = @(x) (waverec2(x(1:ncoef1),S1,'db1')+...
    waverec2(x(ncoef1+1:ncoef1+ncoef2),S2,'db2')+...
    waverec2(x(2*ncoef1+1:2*ncoef1+ncoef2),S3,'db3')+...
    waverec2(x(3*ncoef1+1:3*ncoef1+ncoef2),S4,'db4')+...
    waverec2(x(4*ncoef1+1:4*ncoef1+ncoef2),S5,'db5')+...
    waverec2(x(5*ncoef1+1:5*ncoef1+ncoef2),S6,'db6')+...
    waverec2(x(6*ncoef1+1:6*ncoef1+ncoef2),S7,'db7')+...
    waverec2(x(7*ncoef1+1:7*ncoef1+ncoef2),S8,'db8')+...
    reshape(x(8*ncoef1+1:8*ncoef1+ncoef2), size(im)))/sqrt(9);

% Parameters for BPDN
param1.verbose = 1; % Print log or not
param1.gamma = 1e-3; % Converge parameter
param1.rel_obj = 1e-4; % Stopping criterion for the L1 problem
param1.max_iter = 500; % Max. number of iterations for the L1 problem
param1.nu = 1; % Bound on the norm of the operator A
param1.tight_L1 = 0; % Indicate if Psit is a tight frame (1) or not (0)
param1.max_iter_L1 = 100;
param1.rel_obj_L1 = 1e-3;
param1.pos_L1 = 1;
param1.nu_L1 = 1;
param1.verbose_L1 = 0;
     
% Solve BPDN
tstart = tic;
[sol, z] = sopt_admm_bpcon(y, epsilon, A, At, Psi, Psit, param1);
tend = toc(tstart)

error = im - sol;
SNR = 20*log10(norm(im(:))/norm(error(:)))
residual = At(y - A(sol));
DR = max(sol(:))/(norm(residual(:))/sqrt(N))

% show result
figure, imagesc(im), colorbar, axis image, title('Given image');
figure, imagesc(sol), colorbar, axis image, title('Result of ADMM');
figure, imagesc(log10(im + 1e-4)), colorbar, axis image, title('Log of Given image');
figure, imagesc(log10(sol + 1e-4)), colorbar, axis image, title('Log of result of ADMM');



