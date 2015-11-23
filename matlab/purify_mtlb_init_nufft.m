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
st.Nx1 = imsize(1);
st.Ny1 = imsize(2);
st.Nx2 = FTsize(1);
st.Ny2 = FTsize(2);
%Setup for kernels. Mostly choosing support and widths of kernels.
switch kernelname
    case 'kb'
        %Fessler et al suggests this value of alpha for kb kernels.
        alphau = 2.34*Ju;
        alphav = 2.34*Jv;
        %the zero order kb_kernel
        kb_kernel = @(u, j, alpha, Jtotal) besseli(0,alpha*real(sqrt(1-(2*(u-j)/Jtotal).^2)))/besseli(0,alpha);
        %the ft of the kb kernel
        z = @(u, Jtotal, alpha) sqrt( (2*pi*(Jtotal/2)*u).^2 - alpha^2 );
        ft_kb_kernel = @(x, alpha, Jtotal) real((2*pi)^(1/2) .* (Jtotal/2)^(1/2) ./ besseli(0, alpha) ...
        .* besselj(1/2, z((x), Jtotal, alpha)) ./ z((x), Jtotal, alpha).^(1/2));
        %scaling factor for the image due to the kb kernel
        st.deconv = ft_kb_kernel(X./FTsize(1)-0.5, alphau, Ju).*ft_kb_kernel(Y/FTsize(2)-0.5, alphav, Jv);
        %storing kernels
        kernelu =  @ (u, j) kb_kernel(u,j,alphau, Ju);
        kernelv =  @ (v, j) kb_kernel(v,j,alphav, Jv);
    case 'gauss'
        %Fessler et al suggests this value of sigma for gauss kernels.
        sigmau = (0.31*Ju^(0.52));
        sigmav = (0.31*Jv^(0.52));
        %gaussian kernel
        gauss_kernel = @(u, j, sigma) exp(-(((u-j)/(sigma)).^2)/2);
        %ft of guassian
        ftgauss_kernel = @(x, sigma) exp(-2*((x*sigma).^2))*sigma*sqrt(2*pi);
        %scaling factor for the image due to the gauss kernel
        st.deconv = ftgauss_kernel(X./FTsize(1)-0.5,2*sigmau).*ftgauss_kernel(Y/FTsize(2) - 0.5,2*sigmav);
        %storing kernel
        kernelu =  @ (u, j) gauss_kernel(u, j, sigmau);
        kernelv =  @ (v, j) gauss_kernel(v, j, sigmav);
    case 'minmax:uniform'
        alphau = [1]; % Scaling factors
        betau = 0; % Scaling parameter
        alphav = [1]; % Scaling factors
        betav = 0; % Scaling parameter
        % Generate T from scaling variables alpha and beta.
        [Tu, alphau, betau] = purify_mtlb_init_T_min_max(imsize(1),FTsize(1), Ju, alphau, betau);
        [kernelu, ~, ~] = purify_mtlb_init_min_max(imsize(1), FTsize(1), Ju, Tu, alphau, betau);
        [Tv, alphav, betav] = purify_mtlb_init_T_min_max(imsize(2), FTsize(2), Jv, alphav, betav);
        [kernelv, ~, ~] = purify_mtlb_init_min_max(imsize(2), FTsize(2), Jv, Tv, alphav, betav);
        st.deconv = ones(FTsize);
    otherwise
        Ju = 1;
        Jv = 1;
        kernelu = @ (u, j) 1;
        kernelv = @ (v, j) 1;
        st.deconv = ones(FTsize);
end
st.weights = purify_mtlb_init_interpolation_matrix(UV, kernelu, kernelv, FTsize, Ju, Jv, parallel_type);
end