function st = purify_mtlb_init_ftgrid(freq,Ny1,Nx1,Ky,Kx,tau2,tau1,Ofy,Ofx)

%innit_ftgrid initializes a structure to compute arbitrary Fourier values
%of discrete images. It precomputes and stores the interpolation matrix.
%It uses a truncated Gaussian interpolation kernel.
%Inputs:
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
%Output:
%Structure st wit the following parameters:
% st.weights : sparse interpolation matrix 
% st.Nx1
% st.Ny1
% st.Nx2
% st.Ny2

v2 = freq(:,2);
u2 = freq(:,1);

num_meas=length(v2);

Nx2=Ofx*Nx1;
Ny2=Ofy*Ny1;

%Frequency grid
if mod(Nx2,2)==0
    x = linspace(-pi,pi-(2*pi/Nx2),Nx2);
else
    x = linspace(-((Nx2-1)/2)*(2*pi/Nx2),((Nx2-1)/2)*(2*pi/Nx2),Nx2);
end

if mod(Ny2,2)==0
    y = linspace(-pi,pi-(2*pi/Ny2),Ny2);
else
    y = linspace(-((Ny2-1)/2)*(2*pi/Nx2),((Ny2-1)/2)*(2*pi/Ny2),Ny2);
end

a = x(1)-(Kx/2)*(2*pi/Nx2);
b = x(Nx2)+(Kx/2)*(2*pi/Nx2);
x3 = a:(2*pi/Nx2):b;

a = y(1)-(Ky/2)*(2*pi/Ny2);
b = y(Ny2)+(Ky/2)*(2*pi/Ny2);
y3 = a:(2*pi/Ny2):b;


%weights=sparse(num_meas,Nx2*Ny2);
weights=spalloc(num_meas,Nx2*Ny2,num_meas*Kx*Ky);

fprintf('NUFFT initialization \n\n');

%matlabpool
parfor k=1:num_meas
    
    fprintf('Iteration: %i \n',k);
    
    weights(k,:)=purify_mtlb_init_ftrow(u2(k),v2(k),x,y,x3,y3,tau1,tau2,Kx,Ky,Nx2,Ny2);
        
end

%Spatial grid
if mod(Nx1,2)==0
    x = linspace(-Nx1/2,Nx1/2,Nx1);
else
    x = linspace(-Nx1/2,Nx1/2,Nx1);
end

if mod(Ny1,2)==0
    y = linspace(-Ny1/2,Ny1/2,Ny1);
else
    y = linspace(-Ny1/2,Ny1/2,Ny1);
end

[x2,y2]=meshgrid(x,y);

%Inverse Gaussian to deconvolve the kernel effect

R=exp(((x2*tau1).^2+(y2*tau2).^2)/2);




st.weights=weights;
st.deconv=R;
st.Nx1=Nx1;
st.Ny1=Ny1;
st.Nx2=Nx2;
st.Ny2=Ny2;