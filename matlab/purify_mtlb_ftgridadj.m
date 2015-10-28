function im_est = ftgridadj(vis,st)

Nx1=st.Nx1;
Ny1=st.Ny1;
Nx2=st.Nx2;
Ny2=st.Ny2;

%Gridding
spec_est1=st.weights'*vis;
spec_est1=reshape(spec_est1,Ny2,Nx2);

%Quadrant swap and FFT
spec_est = fftshift(spec_est1);
im21 = ifft2(spec_est)*sqrt(Nx2*Ny2);

%Cropping
%Find image first point (centered image)
xo = floor(Nx2/2) - floor(Nx1/2);
yo = floor(Ny2/2) - floor(Ny1/2);

im_est = st.deconv.*im21(yo+1:yo+Ny1,xo+1:xo+Nx1);
%im_est = im21(1:Ny1,1:Nx1);