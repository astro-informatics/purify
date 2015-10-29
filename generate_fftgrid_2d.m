function [fftgrid] = generate_fftgrid_2d(data_m, omega_m, imsize, fftsize, J, alpha, beta)



% Generate T from scaling variables alpha and beta.
[T, alpha, beta, L] = T_init(fftsize ,imsize, J, alpha, beta);

% Match omega_m values to km (on the grid), and offset them by J/2
km = omega_to_k(fftsize, omega_m, J);

u = zeros(J, length(omega_m), length(fftsize));
% Precompute every interpolation kernel

% Loop over sample locations
for p = 1:length(fftsize)
    for m = 1:length(omega_m)
            [u(:, m, p), ~] = u_init_1d(imsize(p), fftsize(p), J, T(:,:, p), omega_m(m, p), km(m, p), alpha, beta, L);
    end
end


fftgrid = zeros(fftsize); % empty fft grid

% Interpolating fft grid
for q = 1:J
    for p = 1:J
        for m = 1:length(omega_m)
                fftgrid(mod(km(m,1)+fftsize(1)*0.5 + q, fftsize(1)), mod(km(m,2)+fftsize(2)*0.5 + p, fftsize(2))) = fftgrid(mod(km(m,1)+fftsize(1)*0.5 + q,fftsize(1)), mod(km(m,2)+fftsize(2)*0.5 + p,fftsize(2))) + data_m(m)*u(q, m, 1)*u(p, m, 2);
        end
    end
end