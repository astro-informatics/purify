%Tests generating the interpolation kernel in 1D, and creates plots
%for each frequency omega_m (digital).

% This example will plot the same interpolation function in Fig. 7 of
% Fessler et al. 2003, where alpha = [1] and beta = 0.



Ju = 6; % Number of nearest neighbours
imsize = 128; % Image size
FTsize = imsize * 2; % FFT grid size
omega_m = (0:0.001:0.99)' - mod(Ju,2)/2; % Ungridded frequencies.

alphau = [1, -0.2, -0.04]; % Scaling factors
betau =0.34; % Scaling parameter


% Generate T from scaling variables alpha and beta.
[Tu, alphau, betau, Lu] = purify_mtlb_init_T_min_max(imsize(1), FTsize(1), Ju, alphau, betau);

% Match omega_m values to km (on the grid), and offset them by J/2

km = purify_mtlb_omega_to_k(FTsize, omega_m, Ju);

M = length(omega_m); % Number of interpolation kernels to calculate
U = zeros(Ju,length(omega_m));

[minmax, rminmax, r] = purify_mtlb_init_min_max(imsize(1), FTsize(1), Ju, Tu, alphau, betau);

figure;

for m = 1:M
    for i = 1:Ju
        U(i, m) = rminmax(omega_m(m) - km(m), i);
    end
end
hold on
for j = 1:Ju
    plot(omega_m+ Ju/2-j, U(j,:))
end
t = sprintf('j= %d',  (1:Ju) );
title(t)
xlabel('\omega/\gamma') % x-axis label
ylabel('|u_j|') % y-axis label
hold off
% figure;
% for j = 1:Ju
%     subplot(1, Ju, j)
%     plot(omega_m,(U(j,:)))
%     t = sprintf('j = %d',j);
%     title(t)
%     xlabel('\omega/\gamma') % x-axis label
%     ylabel('|u_j|') % y-axis label
% end

[rr, ~] = nufft_r(omega_m*2*pi/FTsize, imsize, Ju, FTsize, alphau, betau, 0);
Uf = zeros(Ju,length(omega_m));
for m = 1:M
        Uf(:, m) = Tu*rr(:,m)-U(:, m);
end
figure
hold on
for j = 1:Ju
    plot(omega_m+ Ju/2-j, Uf(j,:))
end
for m = 1:M
        Uf(:, m) = Tu*rr(:,m);
end
figure
hold on
for j = 1:Ju
    plot(omega_m+ Ju/2-j, Uf(j,:))
end