function mask = genMask(pdf, seed)
% GENMASK - Generate a mask with variable density sampling
% pdf is an array with the desired profile



if nargin==2
    rand('seed', seed);
end

%
mask = rand(size(pdf))<pdf;

end