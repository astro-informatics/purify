%Tests generating the interpolation kernel in 1D, and creates plots
%for each frequency omega_m (digital).


alpha = [1, -0.57, 0.185];
beta = 0.43;
J=10;
imsize= [1024];
fftsize = [2048];
omega_m = ((-4):1:4)';

M = length(omega_m);

% Generate T from scaling variables alpha and beta.
[T, alpha, beta, L] = T_init(fftsize ,imsize, J, alpha, beta);

%
km = omega_to_k(fftsize, omega_m, J);

for m = 1:length(omega_m)
    subplot(ceil(sqrt(M)), ceil(sqrt(M)),m)
    [u, T, r]=u_init_1d(m, imsize, fftsize, J, T, omega_m, km, alpha, beta, L);
    plot(1:J,abs(u))
    t = sprintf('\\omega_m = %f',omega_m(m));
    title(t)
    xlabel('J') % x-axis label
    ylabel('u_J') % y-axis label
end