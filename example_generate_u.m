%Tests generating the interpolation kernel in 1D, and creates plots
%for each frequency omega_m (digital).

% This example will plot the same interpolation function in Fig. 7 of
% Fessler et al. 2003, where alpha = [1] and beta = 0.



J = 6; % Number of nearest neighbours
imsize = 128; % Image size
fftsize = imsize * 2; % FFT grid size
omega_m = (0:0.001:0.99)'; % Ungridded frequencies.

alpha = [1]; % Scaling factors
beta = 0; % Scaling parameter


% Generate T from scaling variables alpha and beta.
[T, alpha, beta, L] = T_init(fftsize ,imsize, J, alpha, beta);

% Match omega_m values to km (on the grid), and offset them by J/2

km = omega_to_k(fftsize, omega_m, J);

M = length(omega_m); % Number of interpolation kernels to calculate
U = zeros(J,length(omega_m));

figure;

for m = 1:M
    [u, realu] = u_init_1d(imsize, fftsize, J, T, omega_m(m), km(m), alpha, beta, L);
    U(:,m) = realu;
end
hold on
for j = 1:J
    plot(omega_m-j, U(j,:))
end
t = sprintf('j= %d',  (1:J) );
title(t)
xlabel('\omega/\gamma') % x-axis label
ylabel('|u_j|') % y-axis label
hold off
figure;
for j = 1:J
    subplot(1, J, j)
    plot(omega_m,abs(U(j,:)))
    t = sprintf('j = %d',j);
    title(t)
    xlabel('\omega/\gamma') % x-axis label
    ylabel('|u_j|') % y-axis label
end