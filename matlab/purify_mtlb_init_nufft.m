function st = purify_mtlb_init_nufft(UV, imsize, oversample_rate, kernelname, J, parallel_type)
%UV are the visibility coordinates
%imsize is the image size
%oversample_factor is the factor of oversampling in the nufft
%kernel is the choice of kernel as a string
%J is the number of nearest neighbours for interpolation kernel

Ju = J(1);
Jv = J(2);
FTsize = imsize*oversample_rate;
[X,Y] = meshgrid(1:FTsize(1),1:FTsize(2));
%Setup for kernels. Mostly choosing support and widths of kernels.
switch kernelname
    case 'kb'
        %Fessler et al suggests this value of alpha for kb kernels.
        alphau = 2.34*Ju;
        alphav = 2.34*Jv;
        %the zero order kb_kernel
        kb_kernel = @(u, j, alpha, Jtotal) besseli(0,alpha*real(sqrt(1-(2*(u+j)/Jtotal).^2)))/besseli(0,alpha);
        %the ft of the kb kernel
        ft_kb_kernel = @(x, x0, alpha, Jtotal) sqrt(pi) * Jtotal/2 * besselj(1/2, sqrt((pi*Jtotal*(x-x0)).^2-alpha^2))./sqrt(sqrt((pi*Jtotal*(x-x0)).^2-alpha^2)/2);
        %scaling factor for the image due to the kb kernel
        st.S = ft_kb_kernel(X./(FTsize(1)), 1/2, alphau, Ju).*ft_kb_kernel(Y/(FTsize(2)), 1/2, alphav, Jv);
        %storing kernels
        kernelu =  @ (u, j) kb_kernel(u,j,alphau, Ju);
        kernelv =  @ (v, j) kb_kernel(v,j,alphav, Jv);
    case 'gauss'
        %Fessler et al suggests this value of sigma for gauss kernels.
        sigmau = (0.31*Ju^(0.52));
        sigmav = (0.31*Jv^(0.52));
        %gaussian kernel
        gauss_kernel = @(u, j, sigma) exp(-(((u+j)/(sigma)).^2)/2);
        %ft of guassian
        ftgauss_kernel = @(x, x0, sigma) exp(-2*(((x-x0)*sigma).^2))*sigma*sqrt(2*pi);
        %scaling factor for the image due to the gauss kernel
        st.S = ftgauss_kernel(X./(FTsize(1)), 1/2,2*sigmau).*ftgauss_kernel(Y/(FTsize(2)), 1/2,2*sigmav);
        %storing kernel
        kernelu =  @ (u, j) gauss_kernel(u, j, sigmau);
        kernelv =  @ (v, j) gauss_kernel(v, j, sigmav);
    case 'minmax'
        alphau = [1]; % Scaling factors
        betau = 0; % Scaling parameter
        alphav = [1]; % Scaling factors
        betav = 0; % Scaling parameter
        % Generate T from scaling variables alpha and beta.
        [Tu, alphau, betau, Lu] = purify_mtlb_init_T_min_max(FTsize(1) ,imsize(1), Ju, alphau, betau);
        [kernelu, ~] = purify_mtlb_init_min_max(imsize(1), FTsize(1), Ju, Tu, alphau, betau, Lu);
        [Tv, alphav, betav, Lv] = purify_mtlb_init_T_min_max(FTsize(2) ,imsize(2), Jv, alphav, betav);
        [kernelv, ~] = purify_mtlb_init_min_max(imsize(2), FTsize(2), Jv, Tv, alphav, betav, Lv);
        st.S = ones(FTsize);
    otherwise
        Ju = 1;
        Jv = 1;
        kernelu = @ (u, j) 1;
        kernelv = @ (v, j) 1;
        st.S = ones(FTsize);
end
st.G = purify_mtlb_init_interpolation_matrix(UV, kernelu, kernelv, FTsize, Ju, Jv, parallel_type);
end