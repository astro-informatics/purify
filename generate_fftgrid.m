function [fftgrid] = generate_fftgrid(data_m, omega_m, imsize, fftsize, J, alpha, beta)



% Generate T from scaling variables alpha and beta.
[T, alpha, beta, L] = T_init(fftsize ,imsize, J, alpha, beta);

% Match omega_m values to km (on the grid), and offset them by J/2
km = omega_to_k(fftsize, omega_m, J);

u = zeros(J, length(omega_m), length(fftsize));
% Precompute every interpolation kernel
% Loop over dimensions
for p = 1:length(fftsize)
    % Loop over sample locations
    for m = 1:length(omega_m(:,p))
        u(:, m, p) = u_init_1d(m, imsize(p), fftsize(p), J, T(:,:,p), omega_m(:,p), km(:,p), alpha, beta, L);
    end
end

Yk = zeros(fftsize); % empty fft grid



% Interpolating fft grid
for m = 1:length(omega_m(:,1))

end
