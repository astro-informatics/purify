clear
Ju = 1;
Jv = Ju;
cellsize = 0.3; %arcsec
imsize = [1024, 1024];
oversample_rate = 2; %most of the kernel parameters are optimized for K/N=2
FTsize = imsize*oversample_rate;
%kernel_type = 'gauss';
kernel_type = 'kb';
%kernel_type = 'minmax:uniform';
stringname = 'at166B.3C129.c0.vis';

Y = importdata(stringname);
U = Y(:,1);
V = Y(:,2);
vis = Y(:,3)+1j*Y(:,4);
%m = sqrt(max(max(U.^2+V.^2)))/3;
m = 180*3600/cellsize/pi;
U = [U;-U]/m*2*pi;
V = [V;-V]/m*2*pi;
vis = [vis; conj(vis)];

st = purify_mtlb_init_nufft([U, V], imsize, oversample_rate, kernel_type, [Ju, Jv]);
fftdata = st.weights' * vis;
fftdata = transpose(reshape(fftdata, FTsize(1), FTsize(2)));

im = ifftshift(ifft2(fftdata))*sqrt(FTsize(1)*FTsize(2));
im_deconv = im.*st.deconv;

c = (FTsize + mod(FTsize,2))/2 - (imsize+mod(imsize,2))/2+1;

im = im(c(1):c(1)+imsize(1)-1, c(2):c(2)+imsize(2)-1);
im_deconv = im_deconv(c(1):c(1)+imsize(1)-1, c(2):c(2)+imsize(2)-1);
figure
realim_deconv = real(im_deconv);
image(realim_deconv/max(max(realim_deconv))*100);

figure
realim = real(im);
image(realim/max(max(realim))*100);
%figure
%imagim = imag(im);
%image(imagim);
realname = sprintf('at166BtestReal_deconvJ%d%s%d.fits',Ju, kernel_type, oversample_rate);
fitswrite(flipud(realim_deconv/max(max(realim_deconv))), realname)
realname = sprintf('at166BtestRealJ%d%s%d.fits',Ju, kernel_type, oversample_rate);
fitswrite(flipud(realim/max(max(realim))), realname)
%imagname = sprintf('at166BtestImagJ%d%s.fits',Ju, kernel_type);
%fitswrite(imagim, imagname)