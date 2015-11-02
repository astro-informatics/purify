% Script that tests guassian gridding


% freg : Mx2 array storing the M digital frequencies in radians [-pi,pi].
% Ny1 : number of rows of the input image.
% Nx1 : number of columns of the input image.
% Ky : interpolation window size in first dimension.
% Kx : interpolation window size in second dimension.
% Kx and Ky should be both even.
% tau2 : variance of the interpolation kernel in first dimension.
% tau1 : variance of the interpolation kernel in second dimension.
% Ofy : oversampling factor in first dimension. Integer >=1.
% Ofx : oversampling factor in second dimension. Integer >=1.
%

im = fitsread('cluster.fits');
Nx1 = size(im, 1);
Ny1 = size(im, 2);
Kx = 4;
Ky = 4;
tau1 = (0.31*Kx^(0.52))/Nx1*pi;
tau2 = tau1;
Ofy = 2;
Ofx = 2;
%% Load data
%stringname='AMI_01.vis';
%Y = importdata(stringname);


[u,v] = ind2sub([Nx1, Ny1], 1:(Nx1*Ny1)) ;
freq = [u'-1,v'-1];
freq = freq/sqrt(Nx1*Ny1)*pi*2 -pi;

% Generating interpolation matrix
st = purify_mtlb_init_ftgrid(freq,Ny1,Nx1,Ky,Kx,tau2,tau1,Ofy,Ofx);

% Generating data off the grid
vis = purify_mtlb_ftgridfwd(im,st);

%Putting data onto the grid
im_est = purify_mtlb_ftgridadj(vis,st);
image(real(im_est)*Nx1*Ny1);