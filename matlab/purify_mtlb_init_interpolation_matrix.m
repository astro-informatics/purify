function interpolation_matrix = purify_mtlb_init_interpolation_matrix(omega_m, kernel, fftsize, Ju, Jv)
%Function to generate interpolation matrix
%omega_m are the coordinates of the visibilities
%kernel is the function for the interpolation kernel
%fftsize is the size of the fft grid
%Ju number of nearest neighbours in axis 1
%Jv number of nearest neighbours in axis 2

 FFTu = fftsize(1);
 FFTv = fftsize(2);
 % visibility coordinates
 
 um = purify_mtlb_omega_to_k(FFTu, omega_m(:,1), Ju);
 vm = purify_mtlb_omega_to_k(FFTv, omega_m(:,2), Jv);
 M = length(um);

 interpolation_matrix = spalloc(M,FFTu*FFTv,M*Ju*Jv);
 %temp_interp = zeros(Ju*Jv, M);
 %temp_index = zeros(Ju*Jv, M);
 for ju = 1:Ju
     i = mod(um+ju,FFTu)+1;
     for jv = 1:Jv
         sprintf('Generating entry (%d %d)', ju, jv)
         j = mod(vm+jv,FFTv)+1;
         cols = sub2ind([FFTu, FFTv], i, j);
         index = sub2ind([M,FFTu*FFTv], 1:M, cols');
         interpolation_matrix(index) =+ kernel(um-omega_m(:,1),ju).*kernel(vm-omega_m(:,2),jv);
     end
 end
end