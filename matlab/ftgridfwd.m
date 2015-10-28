function vis = ftgridfwd(im,st)

Nx1=st.Nx1;
Ny1=st.Ny1;
Nx2=st.Nx2;
Ny2=st.Ny2;

%Zero padding
imc = zeros(Ny2,Nx2);

%Find image first point (centered image)
xo = floor(Nx2/2) - floor(Nx1/2);
yo = floor(Ny2/2) - floor(Ny1/2);

imc(yo+1:yo+Ny1,xo+1:xo+Nx1) = st.deconv.*im;
%imc(1:Ny1,1:Nx1) = im;

%FFT and quadrant swap
spec = fft2(imc)/sqrt(Nx2*Ny2);
spec1 = fftshift(spec);

%Interpolation
vis=st.weights*spec1(:);