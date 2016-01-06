clear
%Linking Nufft
addpath ../../../Software/irt/nufft
addpath ../../../Software/irt/utilities
addpath ../../../Software/irt/systems


%% Load data
stringname='../data/vla/at166B.3C129.c0.vis';
Y = importdata(stringname);

%Measurement vector
vis = Y(:,3) + 1i*Y(:,4);

cellsize = 0.3;

%u-v coverage
U = Y(:,1);
V = Y(:,2);
m = 180/pi*3600/cellsize;
U = [U;-U]/m*2*pi;
V = [V;-V]/m*2*pi;
vis = [vis; conj(vis)];

%Noise standard deviation vector
sigma = Y(:,5);
%Weighting vector
w = 1./sigma;


clear Y

M = length(vis);

%% Measurement operator initialization
%Image dimensions
Nx = 1024;
Ny = 1024;

N = Nx*Ny;

%Oversampling factors for nufft
ox = 2;
oy = 2;

%Number of neighbours for nufft
Ju = 6;
Jv = 6;


%Initialize nufft parameters
fprintf('Initializing the NUFFT operator\n\n');
tstart1=tic;
st = nufft_init([V U],[Ny Nx],[Jv Ju],[oy*Ny ox*Nx], [Ny/2 Nx/2],'kaiser');
%turn off scaling
%st.sn = st.sn * 0 + 1;
tend1=toc(tstart1);
fprintf('Time for the initialization: %e\n\n', tend1);

%Operator functions
A = @(x) nufft(x, st);
At = @(x) nufft_adj(x, st);

dirty = At(vis);
image(real(dirty));
realname = sprintf('at166BtestReal_deconvJ%d%s%dkb.fits',Ju, 'fessler', ox);
fitswrite(flipud(real(dirty))/max(max(real(dirty))), realname);

%turn off scaling
st.sn = st.sn * 0 + 1;
At = @(x) nufft_adj(x, st);

dirty = At(vis);
image(real(dirty));
realname = sprintf('at166BtestRealJ%d%s%dkb.fits',Ju, 'fessler', ox);
fitswrite(flipud(real(dirty))/max(max(real(dirty))), realname);