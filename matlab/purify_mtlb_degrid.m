function vis = purify_mtlb_degrid(im,st)

%Image Dimensions
Nx1=st.Nx1;
Ny1=st.Ny1;
%FTgrid Dimensions
Nx2=st.Nx2;
Ny2=st.Ny2;

%Zero padding
imc = zeros(Ny2,Nx2);

%Find image first point (centered image)
xo = floor(Nx2/2) - floor(Nx1/2);
yo = floor(Ny2/2) - floor(Ny1/2);
imc(yo+1:yo+Ny1,xo+1:xo+Nx1) = im;
imc = imc.*st.gridding_correction;

%FFT to grid
grid = fft2(imc)/sqrt(Nx2*Ny2);

%Interpolation
vis=st.gridding_matrix*grid(:);