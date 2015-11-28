function scale_factor = purify_mtlb_scalefactor_fft2( kernelu, kernelv, X, Y)
% Given

[Nx2, Ny2] = size(X);

scale_factor = fftshift(abs(fft2(ifftshift(kernelu(X).*kernelv(Y)))))/sqrt(Nx2*Ny2);

end

