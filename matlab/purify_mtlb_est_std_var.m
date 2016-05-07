function std_e = purify_mtlb_est_std_var(y)
%EST_STD_VAR Summary of this function goes here
%   Detailed explanation goes here


mask = y; % generate mask
mask(abs(mask) > 0) = 1;
mask(abs(mask) == 0) = 0;
%%
% set low-pass and high-pass filters
w_l = [-0.0106, 0.0329, 0.0308, -0.1870, -0.0280, 0.6309, 0.7148, 0.2304];
w_h = [-0.2304, 0.7148, -0.6309, -0.0280, 0.1870, 0.0308, -0.0329, -0.0106];


% compute convolution using high-pass and low-pass filters
W_l = w_l'*w_l;
y_s = conv2(mask.*y, W_l, 'same'); 

y_s_0 =(1-mask).*y_s + mask.*y; 
W_h = w_h'*w_h;
y_n = conv2(y_s_0, W_h, 'same'); 

% extimate the standard deviation
y_n_b=abs(y_n);
std_e = median(y_n_b(:))/0.6745;


end

