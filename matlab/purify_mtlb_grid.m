function im_est = purify_mtlb_grid(vis,st)

%Image Dimensions
Nx1=st.Nx1;
Ny1=st.Ny1;
%FTgrid Dimensions
Nx2=st.Nx2;
Ny2=st.Ny2;

%Gridding
grid_vis=st.gridding_matrix'*vis;
grid=reshape(grid_vis,Ny2,Nx2);

%Quadrant swap and FFT
im = ifftshift(ifft2(grid))*sqrt(Nx2*Ny2).*st.gridding_correction;

%Cropping
%Find image first point (centered image)
xo = floor(Nx2/2) - floor(Nx1/2);
yo = floor(Ny2/2) - floor(Ny1/2);
im_est = im(yo+1:yo+Ny1,xo+1:xo+Nx1);