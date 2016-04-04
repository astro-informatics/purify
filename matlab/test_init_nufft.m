clear
Ju = 6;
Jv = Ju;
cellsize = 0.3; %arcsec
imsize = [1024, 1024];
oversample_rate = 2; %most of the kernel parameters are optimized for K/N=2
FTsize = imsize*oversample_rate;
%kernel_type = 'gauss';
kernel_type = 'pswf';
%kernel_type = 'minmax:uniform';
stringname = '../data/vla/at166B.3C129.c0.vis';

Y = importdata(stringname);
U = Y(:,1);
V = -Y(:,2);
vis = Y(:,3)+1j*Y(:,4);
%m = sqrt(max(max(U.^2+V.^2)))/3;
m = 180*3600/cellsize/pi;
U = [U;-U]/m*2*pi;
V = [V;-V]/m*2*pi;
vis = [vis; conj(vis)];

st = purify_mtlb_init_nufft([V, U], imsize, oversample_rate, kernel_type, [Ju, Jv]);

At = @(y) purify_mtlb_grid(y,st);

im_deconv = At(vis);

st.gridding_correction = 1 + 0*st.gridding_correction;

At = @(y) purify_mtlb_grid(y,st);

im = At(vis);

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
fitswrite(realim_deconv/max(max(realim_deconv)), realname)
realname = sprintf('at166BtestRealJ%d%s%d.fits',Ju, kernel_type, oversample_rate);
fitswrite(realim/max(max(realim)), realname)
%imagname = sprintf('at166BtestImagJ%d%s.fits',Ju, kernel_type);
%fitswrite(imagim, imagname)