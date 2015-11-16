% test for gridding function
clear
Ju = 1;
Jv = Ju;
imsize = [128, 128];
oversample_rate = 2;
FTsize = imsize*oversample_rate;
alphau = 2.34*Ju;
alphav = 2.34*Jv;
kb_kernel = @(x, j, alpha, Jtotal) besseli(0,alpha*real(sqrt(1-(2*(x+j)/Jtotal).^2)))/besseli(0,alpha);

ftkb_kernel = @(x, x0, alpha, Jtotal) sqrt(pi) * Jtotal/2 * besselj(1/2, sqrt((pi*Jtotal*(x-x0)).^2-alpha^2))./sqrt(sqrt((pi*Jtotal*(x-x0)).^2-alpha^2)/2);

[X,Y] = meshgrid(1:FTsize(1),1:FTsize(2));

%Scaling factor, deconvolution
S = ftkb_kernel(X./(FTsize(1)), 1/2, alphau, Ju).*ftkb_kernel(Y/(FTsize(2)), 1/2, alphav, Jv);

stringname = 'at166B.3C129.c0.vis';
Y = importdata(stringname);
U = Y(:,1);
V = Y(:,2);
vis = Y(:,3)+1j*Y(:,4);
m = sqrt(max(max(U.^2+V.^2)));
U = [U;-U]/m * FTsize(1)/3;
V = [V;-V]/m * FTsize(2)/3;
vis = [vis; conj(vis)];
G = purify_mtlb_init_interpolation_matrix([U,V], @ (x, j) kb_kernel(x,j,alphau, Ju), @ (x, j) kb_kernel(x,j,alphav, Jv), FTsize, Ju, Jv, 2);

fftdata = G' * vis;
fftdata = transpose(reshape(fftdata, FTsize(1), FTsize(2)));

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
realname = sprintf('at166BtestReal_deconvJ%dkbtest.fits',Ju);
fitswrite(realim_deconv, realname)
realname = sprintf('at166BtestRealJ%dkbtest.fits',Ju);
fitswrite(realim, realname)
%imagname = sprintf('at166BtestImagJ%dkb.fits',Ju);
%fitswrite(imagim, imagname)