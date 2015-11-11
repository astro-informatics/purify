% test for gridding function
clear
fftsize = [512, 512];
Ju = 3;
Jv = 3;
tau1 = (0.31*Ju^(0.52));

gauss = @(x, j, sigma) exp(-((x+j)/(2*sigma)).^2);

stringname = 'at166B.3C129.c0.vis';
Y = importdata(stringname);
U = Y(:,1);
V = Y(:,2);
vis = Y(:,3)+1j*Y(:,4);
m = sqrt(max(max(U.^2+V.^2)));
U = [U;-U]/m * fftsize(1)*0.5;
V = [V;-V]/m * fftsize(2)*0.5;
vis = [vis; conj(vis)];
G = purify_mtlb_init_interpolation_matrix([U,V], @ (x, j) gauss(x,j,tau1), fftsize, Ju, Jv);

fftdata = G' * vis;

fftdata = transpose(reshape(fftdata, fftsize(1), fftsize(2)));

figure
realim = ifftshift(real(ifft2(fftdata)));
image(realim/max(max(realim))*sqrt(fftsize(1)*fftsize(2)));
figure
imagim = ifftshift(imag(ifft2(fftdata)));
image(imagim*100);
fitswrite(realim, 'at166BtestReal.fits')
fitswrite(imagim, 'at166BtestImag.fits')