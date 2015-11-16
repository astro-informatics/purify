% test for gridding function
clear
Ju = 1;
Jv = Ju;
imsize = [128, 128];
oversample_rate = 2;
fftsize = imsize*oversample_rate;
sigmau = (0.31*Ju^(0.52));
sigmav = (0.31*Jv^(0.52));

gauss_kernel = @(x, j, sigma) exp(-(((x+j)/(sigma)).^2)/2);
%Find image first point (centered image)

ftgauss_kernel = @(x, x0, sigma) exp(-2*(((x-x0)*sigma).^2))*sigma*sqrt(2*pi);

[X,Y] = meshgrid(1:fftsize(1),1:fftsize(2));

%Scaling factor, deconvolution
S = ftgauss_kernel(X./(fftsize(1)), 1/2,2*sigmau).*ftgauss_kernel(Y/(fftsize(2)), 1/2,2*sigmav);

stringname = 'at166B.3C129.c0.vis';
Y = importdata(stringname);
U = Y(:,1);
V = Y(:,2);
vis = Y(:,3)+1j*Y(:,4);
m = sqrt(max(max(U.^2+V.^2)));
U = [U;-U]/m * fftsize(1)/3;
V = [V;-V]/m * fftsize(2)/3;
vis = [vis; conj(vis)];
G = purify_mtlb_init_interpolation_matrix([U,V], @ (x, j) gauss_kernel(x,j,sigmau), @ (x, j) gauss_kernel(x,j,sigmav), fftsize, Ju, Jv, 2);

fftdata = G' * vis;
fftdata = transpose(reshape(fftdata, fftsize(1), fftsize(2)));

im = ifftshift(ifft2(fftdata));%*sqrt(fftsize(1)*fftsize(2));
im_deconv = im./S;

figure
realim_deconv = real(im_deconv);
image(realim_deconv/max(max(realim_deconv))*100);

figure
realim = real(im);
image(realim/max(max(realim))*100);
figure
imagim = imag(im);
image(imagim);
realname = sprintf('at166BtestReal_deconvJ%d.fits',Ju);
fitswrite(realim_deconv, realname)
realname = sprintf('at166BtestRealJ%d.fits',Ju);
fitswrite(realim, realname)
%imagname = sprintf('at166BtestImagJ%d.fits',Ju);
%fitswrite(imagim, imagname)