function vis = purify_mtlb_ongrid_forward(image, u, v, xsize, ysize)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here


ftgrid = fft2(image);
index = sub2ind([ysize, xsize], u, v);
vis = ftgrid(index);
end

