%Tests generating the interpolation kernel in 1D, and creates plots
%for each frequency omega_m (digital).



J=7; % Number of nearest neighbours
imsize= 128; % Image size
fftsize = imsize*2; % FFT grid size
omega_m = ((-1):0.01:1)'; % Ungridded frequencies.

alpha = [1, -0.5319, 0.1522, -0.0199]; % Scaling factors
beta = 0.6339; % Scaling parameter


% Generate T from scaling variables alpha and beta.
[T, alpha, beta, L] = T_init(fftsize ,imsize, J, alpha, beta);

% Match omega_m values to km (on the grid), and offset them by J/2
km = omega_to_k(fftsize, omega_m, J);

M = length(omega_m); % Number of interpolation kernels to calculate
U = zeros(J,length(omega_m));

% Loop to generate and plot the interpolation kernels 
% for m = 1:M
%     subplot(ceil(sqrt(M)), ceil(sqrt(M)),m)
%     [u, T, r] = u_init_1d(imsize, fftsize, J, T, omega_m(m), km(m), alpha, beta, L);
%     plot(1:J,abs(u))
%     t = sprintf('\\omega_m = %.2f',omega_m(m));
%     title(t)
%     xlabel('J') % x-axis label
%     ylabel('u_J') % y-axis label
% end

figure;

for m = 1:M
    [u, T, r, D] = u_init_1d(imsize, fftsize, J, T, omega_m(m), km(m), alpha, beta, L);
    U(:,m) = u;
end
hold on
for j = 1:J
    plot(omega_m,abs(U(j,:)))
end
t = sprintf('J= %d', 1:J);
title(t)
xlabel('\frac{\omega}{\gamma}') % x-axis label
ylabel('u_J') % y-axis label
hold off
figure;
for j = 1:J
    subplot(1, J, j)
    plot(omega_m,abs(U(j,:)))
    t= sprintf('J = %d',j);
    title(t)
    xlabel('\omega/\gamma') % x-axis label
    ylabel('u_J') % y-axis label
end