function [fftgrid] = generate_fftgrid_1d(data_m, omega_m, imsize, fftsize, J, alpha, beta)



% Generate T from scaling variables alpha and beta.
[T, alpha, beta, L] = T_init(fftsize ,imsize, J, alpha, beta);

% Match omega_m values to km (on the grid), and offset them by J/2
km = omega_to_k(fftsize, omega_m, J);

u = zeros(J, length(omega_m), length(fftsize));
% Precompute every interpolation kernel

% Loop over sample locations
for m = 1:length(omega_m)
    [u(:, m), realu] = u_init_1d(imsize, fftsize, J, T(:,:), omega_m(m), km(m), alpha, beta, L);
end


fftgrid = zeros([fftsize, 1]); % empty fft grid

% Interpolating fft grid
for m = 1:length(omega_m)
    fftgrid(mod(km(m)+fftsize*0.5 + (1:J)', fftsize)) = fftgrid(mod(km(m)+fftsize*0.5 + (1:J)',fftsize)) + data_m(m)*u(: ,m);
    
end
