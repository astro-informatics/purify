function image = purify_mtlb_ongrid_inverse(vis, u, v, xsize, ysize)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

index = sub2ind([ysize, xsize], u, v);
ftgrid = zeros(ysize, xsize);
ftgrid(index) = vis;
image = ifft2(ftgrid);
end

