clc
clear;

%%
addpath src/
addpath data/
addpath ../../sopt/matlab
addpath ../../sopt/matlab/prox_operators
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
%im(im<0)=0;


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

    
Kx = 4;
Ky = 4;
Fa = 2;
% %tau1 = (Fa/Nx1)*2*pi;
% %tau2 = (Fa/Ny1)*2*pi;
% 
% tau1 = (Fa/Nx1);
% tau2 = (Fa/Ny1);


%Initialize nufft parameters
fprintf('Initializing the NUFFT operator\n\n');
tstart1=tic;
st = nufft_init([v1 u1],[Ny1 Nx1],[Ky Kx],[Ny2 Nx2], [Ny1/2 Nx1/2], 'kaiser');
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
y = y0 + noise;

%Size of the problem
M = length(y);
N = Ny1*Nx1;

  
%Maximum eigenvalue of operato A^TA
eval = pow_method(A, At, [Ny1,Nx1], 1e-4, 100, 1);

%Bound for the L2 residual
epsilon = sqrt(M + 2*sqrt(M))*sigma_noise;

%Dirty image
dirty = At(y);
dirty1 = 2*real(dirty)/eval;
%dirty1(dirty1<0) = 0;

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
    reshape(x(8*ncoef1+1:8*ncoef1+N), [Ny1 Nx1]))/sqrt(9);


%Parameters for BPDN
param1.verbose = 1; % Print log or not
param1.gamma = 1e-3; % Converge parameter
param1.rel_obj = 1e-2; % Stopping criterion for the L1 problem
param1.max_iter = 1000; % Max. number of iterations for the L1 problem
param1.nu = eval; % Bound on the norm of the operator A
param1.tight_L1 = 0; % Indicate if Psit is a tight frame (1) or not (0)
param1.max_iter_L1 = 100;
param1.rel_obj_L1 = 1e-2;
param1.pos_L1 = 1;
param1.nu_L1 = 1;
param1.verbose_L1 = 1;

     
%Solve BPDN
tstart = tic;
[sol, z] = admm_bpcon_bls(y, epsilon, A, At, Psi, Psit, param1);
tend = toc(tstart)

soln = sol/max(sol(:));

figure, imagesc(log10(soln + 1e-2)), colorbar, axis image
realname = sprintf('Ex_AMI_fessler_gridding.fits');
fitswrite(real(sol)/max(max(real(sol))), realname)