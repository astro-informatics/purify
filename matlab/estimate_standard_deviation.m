%%% Test, estimate epsilon

%%% Measurements are simulated and given a known value of noise. We then
%%% test how purify_mtlb_est_std_var performs in predicting the noise as a
%%% function of SNR
clear
%Linking Sopt
addpath ../../sopt/matlab/test_images/
addpath ../../sopt/matlab/misc/
addpath ../../sopt/matlab/prox_operators/
addpath ../../sopt/matlab

%Name of file to simulate data from
stringname='M31.fits';

data = fitsread(stringname);
data=flipud(data);
im=im2double(data);
im(im<0)=0;
[Nx,Ny]=size(im);
N = Nx*Ny;

%% Sampling pattern
p = 0.2;
sigma_m = pi/3;
rho = 2-(erf(pi/(sigma_m*sqrt(2))))^2;
num_meas1 = floor(p*rho*N);
u1 = sigma_m*randn(num_meas1,1);
v1 = sigma_m*randn(num_meas1,1);

%Discard points outside (-pi,pi)x(-pi,pi)

sf1=find((u1<pi)&(u1>-pi));
sf2=find((v1<pi)&(v1>-pi));
sf=intersect(sf1,sf2);

%sample pattern coordinates (u, v)
v=v1(sf);
u=u1(sf);
M = length(u);

clear v1 u1


%% Measurement operator initialization 

%Oversampling factors for nufft
ox = 2;
oy = 2;

%Number of neighbours for nufft
Kx = 4;
Ky = 4;

%Initialize nufft parameters
fprintf('Initializing the NUFFT operator\n\n');
tstart1=tic;
st = purify_mtlb_init_nufft([v u],[Ny Nx],ox,'kb',[Ky, Kx]);
tend1=toc(tstart1);
fprintf('Time for the initialization: %e\n\n', tend1);

%Operator functions

A = @(x) purify_mtlb_degrid(x, st);
At = @(x) purify_mtlb_grid(x, st);

%Maximum eigenvalue of operato A^TA
eval = pow_method(A, At, [Ny,Nx], 1e-4, 100, 1);

y0 = A(im); %Simulate measurements
    
% Add Gaussian i.i.d. noise
%sigma_noise = 10^(-input_snr/20)*norm(im(:))/sqrt(N);
snrs = 1:60;
err = snrs;
for i = 1:60
    input_snr = i;
    sigma_noise = 10^(-input_snr/20)*norm(y0)/sqrt(M); % The input standard deviation for given SNR
    noise = (randn(size(y0)) + 1i*randn(size(y0)))*sigma_noise/sqrt(2);
    
    y = y0 + noise;
    %The convolution matrix `st.gridding_matrix` needs to be normalised using the power method ...
    
    grid = st.gridding_matrix' * y; % Convolve sparse data onto a grid, the mask is determined by values that are zero
    
    sigma_estimate = purify_mtlb_est_std_var(grid); %Calculating estimate standard deviation from data
    
    err(i) = round((sigma_noise - std(y))/sigma_noise * 100, 3); % relative error as a percentage
    fprintf('Original: %e\n\nEstimate: %e\n\n', sigma_noise, sigma_estimate);
    fprintf('Relative Error: %f \n\n', round((sigma_noise - sigma_estimate)/sigma_noise, 3));
end
scatter(snrs, err); % Plot of SNR vs. relative error
epsilon = sqrt(M + 2*sqrt(M))*sigma_noise; % Formula for epsilon
