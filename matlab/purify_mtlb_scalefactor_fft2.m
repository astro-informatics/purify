function scale_factor = purify_mtlb_scalefactor_fft2(kernelu, kernelv, X, Y)
% Given two interpolation kernels kernelu and kernelv, it will calculate
% the gridding correction using an FFT.

[Nx2, Ny2] = size(X);

ftgrid = (kernelu((X)).*kernelv((Y)));

scale_factor = abs(fftshift(ifft2(ifftshift(ftgrid))))/sqrt(Nx2*Ny2);

end

